/**
 * Copyright 2013 Eduard Huguet Cuadrench
 * 
 * This file is part of JSMapper kernel module.
 * 
 * JSMapper is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License.
 * 
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with JSMapper.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * \file jsmapper_main.c
 * \brief JSMapper kernel module main file
 * \author Eduard Huguet <eduardhc@gmail.com> 
 */

#include "jsmapper_evgen.h"
#include "jsmapper_core.h"
#include "jsmapper_api.h"
#include "jsmapper_log.h"

#include <linux/kernel.h>
#include <linux/input.h>
#include <linux/slab.h>

#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/major.h>
#include <linux/device.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0)    
	/* From Linux 3.7.0 and onwards a new set of APIs must be used to handle character device allocation */
	#define USE_CDEV
	#include <linux/cdev.h>
#endif


MODULE_AUTHOR("Eduard Huguet <eduardhc@gmail.com>");
MODULE_DESCRIPTION("Joystick mapper device interfaces");
MODULE_LICENSE("GPL");


/**********************************************************************************************************************

	Declarations

**********************************************************************************************************************/

static int 	JSMAPDEV_MAJOR 		= INPUT_MAJOR;
#define 	JSMAPDEV_MINOR_BASE	96
#define 	JSMAPDEV_MINORS		16


/**
 * @brief jsmapper device struct
 *
 * For Linux 3.7 and onwards, a new <b>cdev</b> structure is used to handle character device allocation. Previously, 
 * a simple index and a table holding all the allocated character devices were used.
 */

struct jsmapdev {
	int                     open;
	struct input_handle     handle;
	wait_queue_head_t       wait;
	struct list_head        client_list;
	spinlock_t              client_lock; /* protects client_list */
	struct mutex            mutex;
	struct device           dev;
#ifdef USE_CDEV
	struct cdev				cdev;
#else	
	int                     minor;
#endif	
	bool                    exist;
	struct jsmapdev_core	* core;
};


#ifndef USE_CDEV
	/**
	 * Allocated devices by minor
	 */
	static struct jsmapdev 	* jsmapdev_table[JSMAPDEV_MINORS];
	/**
	 * Protects access to device table
	 */
	static DEFINE_MUTEX(jsmapdev_table_mutex);
#endif	


/**
 * jsmapper device client struct
 */

struct jsmapdev_client {
    int 			head;
	int 			tail;
	spinlock_t 		buffer_lock; /* protects access to buffer, head and tail */
	struct fasync_struct 	*fasync;
	struct jsmapdev 	*jsdev;
	struct list_head 	node;
};



/**********************************************************************************************************************

	Client handling;

**********************************************************************************************************************/

static void jsmapdev_attach_client(struct jsmapdev *jsdev,
					struct jsmapdev_client *client)
{
	spin_lock(&jsdev->client_lock);
	list_add_tail_rcu(&client->node, &jsdev->client_list);
	spin_unlock(&jsdev->client_lock);

    synchronize_rcu();
}

static void jsmapdev_detach_client(struct jsmapdev *jsdev,
					struct jsmapdev_client *client)
{
	spin_lock(&jsdev->client_lock);
	list_del_rcu(&client->node);
	spin_unlock(&jsdev->client_lock);

    synchronize_rcu();
}

static int jsmapdev_open_device(struct jsmapdev *jsdev)
{
	int retval;

	retval = mutex_lock_interruptible(&jsdev->mutex);
	if (retval)
		return retval;

    if (!jsdev->exist) {
        JSMAPPER_LOG_ERROR( "device doesn't exist!" );
		retval = -ENODEV;
    } else {
        jsdev->open++;
        JSMAPPER_LOG_DEBUG( "opening device (%i)", jsdev->open );
        retval = 0;
    }

	mutex_unlock(&jsdev->mutex);
	return retval;
}

static void jsmapdev_close_device(struct jsmapdev *jsdev)
{
	mutex_lock(&jsdev->mutex);

    if (jsdev->exist ) {
        --jsdev->open;
        JSMAPPER_LOG_DEBUG( "closing device(%i)", jsdev->open );
    }

	mutex_unlock(&jsdev->mutex);
}


/**********************************************************************************************************************

	"jsmapper" device file operations

**********************************************************************************************************************/

static int jsmapdev_open(struct inode *inode, struct file *file)
{
	struct jsmapdev_client *client;
	struct jsmapdev *jsdev;
	int i;
	int error;

	JSMAPPER_LOG_DEBUG( "open()" );
	
#ifdef USE_CDEV
	(void) i;
	jsdev = container_of(inode->i_cdev, struct jsmapdev, cdev);
#else	
	i = iminor(inode) - JSMAPDEV_MINOR_BASE;
	if (i >= JSMAPDEV_MINORS)
		return -ENODEV;

	error = mutex_lock_interruptible(&jsmapdev_table_mutex);
	if (error)
		return error;
	jsdev = jsmapdev_table[i];
	if (jsdev)
		get_device(&jsdev->dev);
	mutex_unlock(&jsmapdev_table_mutex);
#endif
	
	if (!jsdev)
		return -ENODEV;

	client = kzalloc(sizeof(struct jsmapdev_client), GFP_KERNEL);
	if (!client) {
		error = -ENOMEM;
		goto err_put_jsdev;
	}

	spin_lock_init(&client->buffer_lock);
	client->jsdev = jsdev;
	jsmapdev_attach_client(jsdev, client);

	error = jsmapdev_open_device(jsdev);
	if (error)
		goto err_free_client;

	file->private_data = client;
	nonseekable_open(inode, file);

	return 0;

 err_free_client:
	jsmapdev_detach_client(jsdev, client);
	kfree(client);
 err_put_jsdev:
	put_device(&jsdev->dev);
	return error;
}

static int jsmapdev_release(struct inode *inode, struct file *file)
{
	struct jsmapdev_client *client = file->private_data;
	struct jsmapdev *jsdev = client->jsdev;

	JSMAPPER_LOG_DEBUG("release()");
	
	jsmapdev_detach_client(jsdev, client);
	kfree(client);

	jsmapdev_close_device(jsdev);

#ifndef USE_CDEV	
	put_device(&jsdev->dev);
#endif	

	return 0;
}


/*
	IOCTL codes:
*/

static int _decode_api_action( struct t_JSMAPPER_ACTION * api_action, struct jsmapdev_core_action * core_action )
{
    int ret = 0;
    int i;
    
    core_action->type = api_action->type;
    switch( core_action->type ) 
    {
    case JSMAPPER_ACTION_DEFAULT:
    case JSMAPPER_ACTION_NONE:
        break;

    case JSMAPPER_ACTION_KEY:
        core_action->key.id = api_action->data.key.id;
        core_action->key.modifiers = api_action->data.key.modifiers;
        core_action->key.single = api_action->data.key.single;
        break;
        
    case JSMAPPER_ACTION_REL:
        core_action->rel.id = api_action->data.rel.id;
        core_action->rel.single = api_action->data.rel.single;
        core_action->rel.step = api_action->data.rel.step;
        core_action->rel.spacing = api_action->data.rel.spacing;
        break;
            
    case JSMAPPER_ACTION_MACRO:
        // TODO check parameter has the appropiate size for the key count passed!
        core_action->macro.spacing = api_action->data.macro.spacing;
        if( api_action->data.macro.count > 0 )  {
            
            size_t cb_keys = sizeof( core_action->macro.keys[0] ) * api_action->data.macro.count;
            core_action->macro.keys = kmalloc( cb_keys, GFP_KERNEL );
            if( core_action->macro.keys ) {
                
                for( i = 0; i < api_action->data.macro.count; i++ ) {
                    core_action->macro.keys[i].id = api_action->data.macro.keys[i].id;
                    core_action->macro.keys[i].modifiers = api_action->data.macro.keys[i].modifiers;
                    core_action->macro.keys[i].single = api_action->data.macro.keys[i].single;
                }
                core_action->macro.count = api_action->data.macro.count;
                
            } else {
                JSMAPPER_LOG_ERROR( "failed to allocate buffer for keys!" );
                ret = -ENOMEM;
            }
        }
        break;
        
    default:
        JSMAPPER_LOG_ERROR( "invalid action type (%u)!", core_action->type );
        ret = -EINVAL;
        break;
    }
    
    return ret;
}

static int _decode_api_button_action( void __user *argp, size_t argp_size, uint * button_id, uint * mode_id, struct jsmapdev_core_button_action * assign )
{
	int ret = 0;
    struct t_JSMAPPER_ACTION * api_action = NULL;
    
    /* copy to kernel space: */
    api_action = (struct t_JSMAPPER_ACTION *) kmalloc( argp_size, GFP_KERNEL );
    if( api_action )  {
        
        ret = copy_from_user( api_action, argp, argp_size );
        if( ret == 0 )
        {
            *mode_id = api_action->mode_id;
            *button_id = api_action->button.id;
            
            assign->filter = api_action->filter;
        
            _decode_api_action( api_action, &assign->action );
        
        } else {
            JSMAPPER_LOG_ERROR( "bad input buffer (error: %i)!", ret );
        }
        
        kfree( api_action );
        
    } else {
        JSMAPPER_LOG_ERROR( "failed to allocate buffer for parameter!" );
        ret = -ENOMEM;
    }
	
	return ret;
}


static int _decode_api_axis_action( void __user *argp, size_t argp_size, uint * axis_id, uint * mode_id, struct jsmapdev_core_axis_action * assign )
{
    int ret = 0;
	struct t_JSMAPPER_ACTION * api_action = NULL;

    /* copy to kernel space: */
    api_action = (struct t_JSMAPPER_ACTION *) kmalloc( argp_size, GFP_KERNEL );
    if( api_action )  {
        
        ret = copy_from_user( api_action, argp, argp_size );
        if( ret == 0 )
        {
            *mode_id = api_action->mode_id;
            *axis_id = api_action->axis.id;
            
            assign->band_low = api_action->axis.low;
            assign->band_high = api_action->axis.high;
            assign->filter = api_action->filter;
            
            _decode_api_action( api_action, &assign->action );
        }
        else
            JSMAPPER_LOG_ERROR( "bad input buffer (error: %i)!", ret );


        kfree( api_action );
        
    } else {
        JSMAPPER_LOG_ERROR( "failed to allocate buffer for parameter!" );
        ret = -ENOMEM;
    }
	

	return ret;
}


static int jsmapdev_ioctl_common( struct jsmapdev *jsdev, unsigned int cmd, void __user *argp )
{
	struct input_dev 							*dev = jsdev->handle.dev;
	size_t 										len = 0;
	char										* name = NULL;
	int                                         mode_id = 0, button_id = 0, axis_id = 0;
    __s32                                       value = 0;
	struct jsmapdev_core_button_action          btn_assign;
	struct jsmapdev_core_axis_action            axis_assign;
	struct t_JSMAPPER_MODE                      mode_p = {0};
	int											ret = 0;

	
	if( jsdev->core == NULL ) {
		JSMAPPER_LOG_WARNING( "not initialized!" );
		return -EFAULT;
	}
	
		
	/* Process fixed-sized commands. */
	switch (cmd) 
	{
	case JMIOCGVERSION:
		return put_user( JSMAPPER_API_VERSION, (__u32 __user *) argp );

	case JMIOCGAXES:
		return put_user( (__u8) jsdev->core->axis_count, (__u8 __user *) argp);

	case JMIOCGBUTTONS:
		return put_user( (__u8) jsdev->core->button_count, (__u8 __user *) argp);
		
	case JMIOCGBUTTONVALUE:
        ret = get_user( value, (__s32 __user *) argp );
        if( ret == 0 ) {
            button_id = jsmapper_core_rmap_button( jsdev->core, value );
            if( button_id >= 0 ) {
                value = test_bit( button_id, jsdev->handle.dev->key )? 1 : 0;
                ret = put_user( value, (__s32 __user *) argp );
            } else {
                JSMAPPER_LOG_ERROR( "Invalid button ID (%i)!", (int) value );
                ret = -EINVAL;
            }
        }
		return ret;
        
    case JMIOCGAXISVALUE:
        ret = get_user( value, (__s32 __user *) argp );
        if( ret == 0 ) {
            axis_id = jsmapper_core_rmap_axis( jsdev->core, value );
            if( axis_id >= 0 ) {
                value = input_abs_get_val( jsdev->handle.dev, axis_id );
                ret = put_user( value, (__s32 __user *) argp );
            } else {
                JSMAPPER_LOG_ERROR( "Invalid axis ID (%i)!", (int) value );
                ret = -EINVAL;
            }
        }
		return ret;

        
	case JMIOCCLEAR:
		jsmapper_core_clear( jsdev->core, 1 );
		return 0;
		
    case JMIOCADDMODE:
        ret = copy_from_user( &mode_p, argp, sizeof( mode_p ) );
        if( ret == 0 ) {
            ret = jsmapper_core_add_mode( jsdev->core, &mode_p );
            if( ret == 0 ) {
                /* copy result back to user, so it can receive new mode ID: */
                ret = copy_to_user( argp, &mode_p, sizeof( mode_p ));
            }
        }
        return ret;
	}


	/* Process variable-sized commands */
	switch( cmd & ~IOCSIZE_MASK )
	{
	case JMIOCGNAME( 0 ):
		name = (char *) dev->name;
		if (!name)
			return 0;

		len = min_t(size_t, _IOC_SIZE(cmd), strlen(name) + 1);
		return copy_to_user(argp, name, len) ? -EFAULT : len;

	case JMIOCGPROFILENAME( 0 ):
		name = jsmapper_core_get_profile_name( jsdev->core );
		if (!name)
			return 0;

		len = min_t(size_t, _IOC_SIZE(cmd), strlen(name) + 1);
		return copy_to_user(argp, name, len) ? -EFAULT : len;

	case JMIOCSPROFILENAME( 0 ):
		len = _IOC_SIZE( cmd );	// TODO check for negatives or too big values...
		name = kmalloc( len + 1, GFP_KERNEL );
		if( name ) {
			ret = copy_from_user( name, argp, len );
			if( ret == 0 ) {
				name[ len ] = '\0';
				JSMAPPER_LOG_INFO( "set profile name: %s", name? name : "(null)" );
				jsmapper_core_set_profile_name( jsdev->core, name );
			} else {
				JSMAPPER_LOG_ERROR( "failed to copy dat from userspace (%u bytes)!", (uint) len );
			}
		} else {
			JSMAPPER_LOG_ERROR( "failed to allocate memory (%u bytes)!", (uint) len );
			ret = -ENOMEM;
		}
		return ret;

    case JMIOCSBUTTONACTION( 0 ):
        len = _IOC_SIZE( cmd );
        if( len >= sizeof( struct t_JSMAPPER_ACTION )) {
            jsmapper_core_init_action( &btn_assign.action );
            ret = _decode_api_button_action( argp, len, &button_id, &mode_id, &btn_assign );
            if( ret == 0 ) {
                ret = jsmapper_core_set_button_action( jsdev->core, button_id, mode_id, &btn_assign );
            }
            jsmapper_core_clear_action( &btn_assign.action );
        
        } else {
            JSMAPPER_LOG_ERROR( "Invalid parameter size (%u)!", (uint) len );
        }
        return ret;
        
    case JMIOCSAXISACTION( 0 ):
        len = _IOC_SIZE( cmd );
        if( len >= sizeof( struct t_JSMAPPER_ACTION )) {
            jsmapper_core_init_action( &axis_assign.action );
            ret = _decode_api_axis_action( argp, _IOC_SIZE( cmd ), &axis_id, &mode_id, &axis_assign );
            if( ret == 0 ) {
                ret = jsmapper_core_set_axis_action( jsdev->core, axis_id, mode_id, &axis_assign );
            }
            jsmapper_core_clear_action( &axis_assign.action );
            
        } else {
            JSMAPPER_LOG_ERROR( "Invalid parameter size (%u)!", (uint) len );
        }
        return ret;
	}

	return -EINVAL;
}


static long jsmapdev_ioctl( struct file *file, unsigned int cmd, unsigned long arg )
{
	struct jsmapdev_client *client = file->private_data;
	struct jsmapdev *jsdev = client->jsdev;
	void __user *argp = (void __user *)arg;
	int retval;

	// JSMAPPER_LOG_DEBUG( "ioctl( cmd=%u, arg=%lu )", cmd, arg );
	
	retval = mutex_lock_interruptible( &jsdev->mutex );
	if (retval)
		return retval;

	if (jsdev->exist) {
		retval = jsmapdev_ioctl_common( jsdev, cmd, argp );
	} else {
		retval = -ENODEV;
	}

	mutex_unlock( &jsdev->mutex );
	return retval;
}


#ifdef CONFIG_COMPAT
static long jsmapdev_ioctl_compat( struct file *file, unsigned int cmd, unsigned long arg )
{
	struct jsmapdev_client *client = file->private_data;
	struct jsmapdev *jsdev = client->jsdev;
	void __user *argp = (void __user *)arg;
	int retval;

	// JSMAPPER_LOG_DEBUG("ioctl_compat( cmd=%u, arg=%lu )", cmd, arg );
	
	retval = mutex_lock_interruptible(&jsdev->mutex);
	if (retval)
		return retval;

	if (jsdev->exist) {
		retval = jsmapdev_ioctl_common( jsdev, cmd, argp );
	} else {
		retval = -ENODEV;
	}

	mutex_unlock(&jsdev->mutex);
	return retval;
}
#endif



/**
 * File operations function pointers for jsmapperdev device
 */
static const struct file_operations jsmapdev_fops = {
	.owner		= THIS_MODULE,
	.open		= jsmapdev_open,
	.release	= jsmapdev_release,
	.unlocked_ioctl	= jsmapdev_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl	= jsmapdev_ioctl_compat,
#endif
	.llseek		= noop_llseek,
};



/**********************************************************************************************************************

	Input handler implementation

**********************************************************************************************************************/

static int jsmapdev_install_chrdev(struct jsmapdev *jsdev)
{
	int error = 0;
	
#ifdef USE_CDEV
	cdev_init(&jsdev->cdev, &jsmapdev_fops);
	jsdev->cdev.kobj.parent = &jsdev->dev.kobj;
	error = cdev_add(&jsdev->cdev, jsdev->dev.devt, 1);
#else	
	/*
	 * No need to do any locking here as calls to connect and
	 * disconnect are serialized by the input core
	 */
	jsmapdev_table[jsdev->minor] = jsdev;
#endif	
	
    return error;
}

static void jsmapdev_remove_chrdev(struct jsmapdev *jsdev)
{
#ifdef USE_CDEV
	cdev_del(&jsdev->cdev);
#else
	/*
	 * Lock evdev table to prevent race with jsmapdev_open()
	 */
	mutex_lock(&jsmapdev_table_mutex);
	jsmapdev_table[jsdev->minor] = NULL;
	mutex_unlock(&jsmapdev_table_mutex);
#endif	
}


/**
 * Marks device non-existent. This disables writes, ioctls and
 * prevents new users from opening the device. Already posted
 * blocking reads will stay, however new ones will fail.
 */
static void jsmapdev_mark_dead(struct jsmapdev *jsdev)
{
	mutex_lock(&jsdev->mutex);
	jsdev->exist = false;
	mutex_unlock(&jsdev->mutex);
}

/**
 * Wake up users waiting for IO so they can disconnect from
 * dead device.
 */

static void jsmapdev_hangup(struct jsmapdev *jsdev)
{
	struct jsmapdev_client *client;

	spin_lock(&jsdev->client_lock);
	list_for_each_entry(client, &jsdev->client_list, node)
		kill_fasync(&client->fasync, SIGIO, POLL_HUP);
	spin_unlock(&jsdev->client_lock);

	wake_up_interruptible(&jsdev->wait);
}


static void jsmapdev_cleanup(struct jsmapdev *jsdev)
{
	struct input_handle *handle = &jsdev->handle;

	jsmapdev_mark_dead(jsdev);
	jsmapdev_hangup(jsdev);
	jsmapdev_remove_chrdev(jsdev);

	/* jsdev is marked dead so no one else accesses jsdev->open */
	if (jsdev->open) {
		input_close_device(handle);
	}
}


static void jsmapdev_free(struct device *dev)
{
	struct jsmapdev *jsdev = container_of(dev, struct jsmapdev, dev);

	JSMAPPER_LOG_DEBUG( "free");
	
	input_put_device(jsdev->handle.dev);
	jsmapper_core_done( jsdev->core );
	
	kfree(jsdev);
}


/**
 * \brief Joystick connection function
 * 
 * It creates new jsmapdev device. Note that input core serializes calls
 * to connect and disconnect so we don't need to lock jsmapdev_table here.
 */

static int jsmapdev_connect( struct input_handler *handler, 
                                struct input_dev *dev,
                                const struct input_device_id *id)
{
	struct jsmapdev *jsdev;
	int minor = 0, dev_no = 0;
	int error = 0;

	JSMAPPER_LOG_DEBUG( "connect( name='%s', vendorID=%i, productID=%i)", dev->name, (int) dev->id.vendor, (int) dev->id.product );

#ifdef USE_CDEV	
	minor = input_get_new_minor(JSMAPDEV_MINOR_BASE, JSMAPDEV_MINORS, true);
	if (minor < 0) {
		error = minor;
		JSMAPPER_LOG_ERROR("failed to reserve new minor: %d\n", error);
		return error;
	}
#else	
	for (minor = 0; minor < JSMAPDEV_MINORS; minor++)
		if (!jsmapdev_table[minor])
			break;

	if (minor == JSMAPDEV_MINORS) {
		JSMAPPER_LOG_ERROR( "no more free jsmapdev devices");
		return -ENFILE;
	}
#endif	

	jsdev = kzalloc(sizeof(struct jsmapdev), GFP_KERNEL);
	if (!jsdev) {
		error = -ENOMEM;
		goto err_free_minor;
	}

	INIT_LIST_HEAD(&jsdev->client_list);
	spin_lock_init(&jsdev->client_lock);
	mutex_init(&jsdev->mutex);
	init_waitqueue_head(&jsdev->wait);

	dev_no = minor;
	/* Normalize device number if it falls into legacy range */
	if (dev_no < JSMAPDEV_MINOR_BASE + JSMAPDEV_MINORS)
		dev_no -= JSMAPDEV_MINOR_BASE;
	dev_set_name(&jsdev->dev, "jsmap%d", dev_no);
	jsdev->exist = true;
#ifndef USE_CDEV	
	jsdev->minor = minor;
#endif	

	jsdev->handle.dev = input_get_device(dev);
	jsdev->handle.name = dev_name(&jsdev->dev);
	jsdev->handle.handler = handler;
	jsdev->handle.private = jsdev;

	jsdev->dev.devt = MKDEV(JSMAPDEV_MAJOR, minor);
	jsdev->dev.class = &input_class;
	jsdev->dev.parent = &dev->dev;
	jsdev->dev.release = jsmapdev_free;
	device_initialize(&jsdev->dev);
	
	/* initialize core struct: */
	jsdev->core = jsmapper_core_init( dev );
	if( !jsdev->core ) {
		error = -ENOMEM;
		goto err_free_jsmapdev;
	}

	error = input_register_handle( &jsdev->handle );
	if (error) {
		JSMAPPER_LOG_ERROR( "failed to register handler for device!");
		goto err_free_jsmapdev;
	}

	error = jsmapdev_install_chrdev(jsdev);
	if (error) {
		JSMAPPER_LOG_ERROR( "failed to install chrdev!");
		goto err_unregister_handle;
	}

	error = device_add( &jsdev->dev );
	if (error)  {
		JSMAPPER_LOG_ERROR( "failed to add device!" );
		goto err_cleanup_jsmapdev;
	}
    
	/* open device: if not, we can't access its state */
	error = input_open_device( &jsdev->handle );
	if( error ) {
		JSMAPPER_LOG_ERROR( "failed to open input device!" );
		goto err_cleanup_open;
	}
	
	return 0;

err_cleanup_open:
	// ???
err_cleanup_jsmapdev:
	jsmapdev_cleanup(jsdev);
err_unregister_handle:
	input_unregister_handle(&jsdev->handle);
err_free_jsmapdev:
	put_device(&jsdev->dev);
err_free_minor:
#ifdef USE_CDEV	
    input_free_minor(minor);
#endif	
	return error;
}


/**
 * \brief Joystick disconnection function
 */

static void jsmapdev_disconnect(struct input_handle *handle)
{
    struct jsmapdev *jsdev = handle->private;
	
	JSMAPPER_LOG_DEBUG( "disconnect");

	input_close_device( &jsdev->handle );
	
	device_del(&jsdev->dev);
	jsmapdev_cleanup(jsdev);
#ifdef USE_CDEV
	input_free_minor( MINOR(jsdev->dev.devt) );
#endif
	input_unregister_handle( handle );

	put_device(&jsdev->dev);
}



/**
 * Input device matching function
 * 
 * (flagrantly copied from joydev.c)
 */

static bool jsmapdev_match(struct input_handler *handler, struct input_dev *dev)
{
	JSMAPPER_LOG_DEBUG( "match( name='%s', vendorID=%i, productID=%i)", dev->name, (int) dev->id.vendor, (int) dev->id.product );

	if( dev == jsmapper_evgen_device() ) {
		JSMAPPER_LOG_DEBUG( "No match (0)" );
		return false;
	}
		
	/* Avoid touchpads and touchscreens */
	if (test_bit(EV_KEY, dev->evbit) && test_bit(BTN_TOUCH, dev->keybit)) {
		JSMAPPER_LOG_DEBUG( "No match" );
		return false;
	}

	/* Avoid tablets, digitisers and similar devices */
	if (test_bit(EV_KEY, dev->evbit) && test_bit(BTN_DIGI, dev->keybit)) {
		JSMAPPER_LOG_DEBUG( "No match (2)" );
		return false;
	}
	
	JSMAPPER_LOG_DEBUG( "Match!" );
	return true;
}


static bool jsmapdev_filter( struct input_handle *handle, unsigned int type, unsigned int code, int value )
{
	bool                                    filter = false;
	struct jsmapdev                         * jsdev = handle->private;
    int										button_id = -1;
    struct jsmapdev_core_button_action      * button_assign = NULL;
    int										axis_id = -1;
    struct jsmapdev_core_axis_action		* axis_assign = NULL;
    struct jsmapdev_core_axis_action		* cur_axis_assign = NULL;
    
    switch( type )	{
    case EV_KEY:
        button_id = jsmapper_core_map_button( jsdev->core, code );
        if( button_id >= 0 ) {
            // JSMAPPER_LOG_DEBUG("filter( button ID=%u, value=%i )", button_id, value );
            
            button_assign = jsmapper_core_find_button_action( jsdev->core, jsdev->core->root_mode, button_id );
            if( button_assign ) {
                jsmapper_evgen_send_action( &button_assign->action, value );
                filter = button_assign->filter;
            }
        }
        else
            JSMAPPER_LOG_WARNING( "unknown key code: 0x%x!", (uint) code );
        break;
        
    case EV_ABS:
        axis_id = jsmapper_core_map_axis( jsdev->core, code );
        if( axis_id >= 0 ) {
            // JSMAPPER_LOG_DEBUG( "filter( axis ID=%u, value=%i )", axis_id, value );

            cur_axis_assign = jsdev->core->current_axis_action[ axis_id ];
            axis_assign = jsmapper_core_find_axis_action( jsdev->core, jsdev->core->root_mode, axis_id, value );
            if( axis_assign != cur_axis_assign ) {
                
                if( cur_axis_assign ) {
                    JSMAPPER_LOG_DEBUG( "Deactivating old action for axis ID=%u", axis_id );
                    jsmapper_evgen_send_action( &cur_axis_assign->action, 0 );
                    if( cur_axis_assign->filter )
                        filter = true;
                }
            
                jsdev->core->current_axis_action[ axis_id ] = axis_assign;
                        
                if( axis_assign ) {
                    JSMAPPER_LOG_DEBUG( "Activating new action for axis ID=%u", axis_id );
                    jsmapper_evgen_send_action( &axis_assign->action, 1 );
                    if( axis_assign->filter )
                        filter = true;
                }
            }
        }
        else
            JSMAPPER_LOG_WARNING( "unknown axis code: 0x%x!", (uint) code );
        break;
        
    default:
        break;
    }
	
	return filter;
}


/**
  * Input device IDs for whose our device matches
  *
  * (flagrantly copied from joydev.c)
  */

static const struct input_device_id jsmapdev_ids[] = {
	{
		.flags = INPUT_DEVICE_ID_MATCH_EVBIT |
				INPUT_DEVICE_ID_MATCH_ABSBIT,
		.evbit = { BIT_MASK(EV_ABS) },
		.absbit = { BIT_MASK(ABS_X) },
	},
	{
		.flags = INPUT_DEVICE_ID_MATCH_EVBIT |
				INPUT_DEVICE_ID_MATCH_ABSBIT,
		.evbit = { BIT_MASK(EV_ABS) },
		.absbit = { BIT_MASK(ABS_WHEEL) },
	},
	{
		.flags = INPUT_DEVICE_ID_MATCH_EVBIT |
				INPUT_DEVICE_ID_MATCH_ABSBIT,
		.evbit = { BIT_MASK(EV_ABS) },
		.absbit = { BIT_MASK(ABS_THROTTLE) },
	},
	{
		.flags = INPUT_DEVICE_ID_MATCH_EVBIT |
				INPUT_DEVICE_ID_MATCH_KEYBIT,
		.evbit = { BIT_MASK(EV_KEY) },
		.keybit = {[BIT_WORD(BTN_JOYSTICK)] = BIT_MASK(BTN_JOYSTICK) },
	},
	{
		.flags = INPUT_DEVICE_ID_MATCH_EVBIT |
				INPUT_DEVICE_ID_MATCH_KEYBIT,
		.evbit = { BIT_MASK(EV_KEY) },
		.keybit = { [BIT_WORD(BTN_GAMEPAD)] = BIT_MASK(BTN_GAMEPAD) },
	},
	{
		.flags = INPUT_DEVICE_ID_MATCH_EVBIT |
				INPUT_DEVICE_ID_MATCH_KEYBIT,
		.evbit = { BIT_MASK(EV_KEY) },
		.keybit = { [BIT_WORD(BTN_TRIGGER_HAPPY)] = BIT_MASK(BTN_TRIGGER_HAPPY) },
	},
	{ }	/* Terminating entry */
	
};


MODULE_DEVICE_TABLE(input, jsmapdev_ids);


/**
  * Input handler definition structure
  */

static struct input_handler jsmapdev_handler = {
	.filter		= jsmapdev_filter,
	.match		= jsmapdev_match,
	.connect	= jsmapdev_connect,
	.disconnect	= jsmapdev_disconnect,
#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,6,0)    
	.fops		= &jsmapdev_fops,
#endif    
	.minor		= JSMAPDEV_MINOR_BASE,
	.name		= "jsmapdev",
	.id_table	= jsmapdev_ids
};



/**********************************************************************************************************************

	Module initialization / cleanup functions

**********************************************************************************************************************/

static int __init jsmapdev_init(void)
{
	int ret = 0;
	
	JSMAPPER_LOG_INFO("init()");

	/* register our input handler: */
	ret = input_register_handler( &jsmapdev_handler );
	if( ret )
	{
		JSMAPPER_LOG_ERROR( "failed to register input handler!" );
		return ret;
	}
	
	/* initialize event generator: */
	ret = jsmapper_evgen_init();
	if( ret )
	{
		JSMAPPER_LOG_ERROR( "failed to create virtual event generator!" );
		return ret;
	}
	
	return 0;
}

static void __exit jsmapdev_exit(void)
{
	JSMAPPER_LOG_INFO("exit()");

	jsmapper_evgen_done();
	input_unregister_handler( &jsmapdev_handler );
}

module_init(jsmapdev_init);
module_exit(jsmapdev_exit);
