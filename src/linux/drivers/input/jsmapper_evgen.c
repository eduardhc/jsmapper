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
 * \file jsmapper_evgen.c
 * \brief JSMapper kernel module event generator
 * \author Eduard Huguet <eduardhc@gmail.com> 
 */

#include "jsmapper_evgen.h"
#include "jsmapper_api.h"
#include "jsmapper_log.h"

#include <linux/input.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/kthread.h> 
#include <linux/sched.h>    

/*************************************************************************************************************

    Static data

**************************************************************************************************************/

/** The event generator virtual device */
static struct input_dev * g_evgen_dev = NULL;

/**
  * \brief Parameter struct for _send_rel() call
  *
  * Contains the parameters used to ctrl the background thread used to generate relative axis movement events.
  */

struct send_rel_thread_params {
    /** axis ID (REL_xxx) */
    uint id;
	/** Number of steps to perform per movement*/
	int step;
    /** Time spacing between steps, in ms */
    uint  spacing;
    /** Cancellation flag */
    int cancel;
};

/** array containing parameters struct for all relative axis actions currently being executed */
struct send_rel_thread_params * g_evgen_rel[REL_CNT];

/** protects certain parts of event generator */
spinlock_t g_evgen_lock; 


/**
 * @brief Wrapper for the send_rel_params work item
 *This structure hols a workqueue control item plus the params for the axis action thread. 
 */
struct send_rel_work_params {
	/** Workqueue control item - required */
    struct work_struct work;
	/** Non-zero if button was being pressed, 0 if released */
	int press;
	/** Thread params */
	struct send_rel_thread_params rel;
};


/**
  * \brief Parameter struct for send_macro() call
  *
  * Contains a 'work_struct' item on top, in order to be inserted into a workqueue.
  */
struct send_macro_work {
    /** Workqueue control item - required */
    struct work_struct work;
    /** Macro definition */
    struct jsmapdev_core_macro macro;
};

/** The work queue used for macros */
static struct workqueue_struct  * g_evgen_wq = NULL;


/*************************************************************************************************************

    Device creation, etc...

**************************************************************************************************************/

int jsmapper_evgen_init()
{
	int result = 0;
	uint id = 0;
    int i = 0;
		
	JSMAPPER_LOG_INFO( "creating virtual event generator..." );
	if( g_evgen_dev ) {
		JSMAPPER_LOG_WARNING( "yet created!" );
		return -EBUSY;
	}
	
	g_evgen_dev = input_allocate_device();
	if( g_evgen_dev == NULL ) {
		JSMAPPER_LOG_ERROR( "failed to allocate device!" );
		return -ENOMEM;
	}
	
	/* init spinlock */
	spin_lock_init( &g_evgen_lock );
	
	/* set up descriptive labels */
	g_evgen_dev->name = "JSMapper virtual event generator";
	/* phys is unique on a running system */
	g_evgen_dev->phys = "jsmapper/evgen/0";
	g_evgen_dev->id.bustype = BUS_VIRTUAL;

	/* set device bits for keyboard events generation: */
	set_bit( EV_KEY, g_evgen_dev->evbit );
	for( id = 0; id < BTN_MISC; id++ )
		set_bit( id, g_evgen_dev->keybit );
    
    /* set device bits for mouse events generation: */
    set_bit( BTN_LEFT, g_evgen_dev->keybit );
    set_bit( BTN_MIDDLE, g_evgen_dev->keybit );
    set_bit( BTN_RIGHT, g_evgen_dev->keybit );
    
    set_bit( EV_REL, g_evgen_dev->evbit );
    for( id = 0; id < REL_MAX; id++ )
		set_bit( id, g_evgen_dev->relbit );
    
	/* and finally register with the input core */
	result = input_register_device( g_evgen_dev );
	if( result ) {
		JSMAPPER_LOG_ERROR( "failed to register device!" );
		
		input_free_device( g_evgen_dev );
		g_evgen_dev = NULL;
		return result;
	}

    /* create workqueue for key sequence generation: */
    g_evgen_wq = create_workqueue( "jsmapper_evgen_wq" );
    if( g_evgen_wq == NULL ) {
        JSMAPPER_LOG_ERROR( "failed to create workqueue !" );
    }
        
    /* initialize relative axes status data: */
    for( i = 0; i < REL_CNT; i++ ) {
        g_evgen_rel[i] = NULL;
    }
    
	return 0;
}


struct input_dev * jsmapper_evgen_device( void )
{
	return g_evgen_dev;
}


void jsmapper_evgen_done( )
{
    int i = 0;
    
    /* destroy workqueue used for macros */
    if( g_evgen_wq )  {
        JSMAPPER_LOG_DEBUG( "destroying workqueue" );
        
        flush_workqueue( g_evgen_wq );
        destroy_workqueue( g_evgen_wq );
        
        g_evgen_wq = NULL;
    }

    /* cancel any relative axis thread that might be still running: */
	spin_lock( &g_evgen_lock );
    for( i = 0; i < REL_CNT; i++ ) {
        if( g_evgen_rel[i] ) {
            JSMAPPER_LOG_WARNING( "Stopping thread for relative axis ID=%u!", i );
            g_evgen_rel[i]->cancel = 1;
            g_evgen_rel[i] = NULL;
            /* WARNING how to ensure thread stops before device is destroyed? Maybe we should simply kill the thread? */
        }
    }
	spin_unlock( &g_evgen_lock );

    /* finally, destroy event generator device */
	if( g_evgen_dev ) {
        JSMAPPER_LOG_INFO( "destroying virtual event generator" );
		
        /*  NOTE no need to call input_free_device() as, according to doc, this is not needed 
            for registered devices (input_unregister_device() will free it)
		 */
        input_unregister_device( g_evgen_dev );	
        
		g_evgen_dev = NULL;
	}
}


/*************************************************************************************************************

    Event generation functions:

**************************************************************************************************************/

int jsmapper_evgen_send_action( struct jsmapdev_core_action * action, int press )
{
	int result = -EINVAL;
	
	if( action ) {
		switch( action->type ) {
		case JSMAPPER_ACTION_KEY:
            result = jsmapper_evgen_send_key( &action->key, press );
			break;
            
        case JSMAPPER_ACTION_REL:
            result = jsmapper_evgen_send_rel( &action->rel, press );
            break;
                
		case JSMAPPER_ACTION_MACRO:
            result = jsmapper_evgen_send_macro( &action->macro, press );
            break;
                
		default:
			break;
		}
	}
	
	return result;
}


/****************************************************************************************************************
  
  Key actions
  
****************************************************************************************************************/

static void _send_key_modifiers( uint modifiers, int press )
{
	if( modifiers & JSMAPPER_MODIFIER_CTRL_L )
		input_event( g_evgen_dev, EV_KEY, KEY_LEFTCTRL, press );
	
	if( modifiers & JSMAPPER_MODIFIER_CTRL_R )
		input_event( g_evgen_dev, EV_KEY, KEY_RIGHTCTRL, press );

	
	if( modifiers & JSMAPPER_MODIFIER_SHIFT_L )
		input_event( g_evgen_dev, EV_KEY, KEY_LEFTSHIFT, press );
	
	if( modifiers & JSMAPPER_MODIFIER_SHIFT_R )
		input_event( g_evgen_dev, EV_KEY, KEY_RIGHTSHIFT, press );

	
	if( modifiers & JSMAPPER_MODIFIER_ALT_L )
		input_event( g_evgen_dev, EV_KEY, KEY_LEFTALT, press );
	
	if( modifiers & JSMAPPER_MODIFIER_ALT_R )
		input_event( g_evgen_dev, EV_KEY, KEY_RIGHTALT, press );

	
	if( modifiers & JSMAPPER_MODIFIER_META_L )
		input_event( g_evgen_dev, EV_KEY, KEY_LEFTMETA, press );
	
	if( modifiers & JSMAPPER_MODIFIER_META_R )
		input_event( g_evgen_dev, EV_KEY, KEY_RIGHTMETA, press );
}


int jsmapper_evgen_send_key( struct jsmapdev_core_key * key, int press )
{
    if( g_evgen_dev == NULL ) {
        JSMAPPER_LOG_ERROR( "event generator device not available!" );
		return -ENODEV;
	}

    if( key->single ) {
        
        if( press ) {
            JSMAPPER_LOG_DEBUG( "sending single key ID=%u, mod=0x%x", key->id, key->modifiers );
            
            _send_key_modifiers( key->modifiers, 1 );
            input_event( g_evgen_dev, EV_KEY, key->id, 1 );
            input_event( g_evgen_dev, EV_KEY, key->id, 0 );
            _send_key_modifiers( key->modifiers, 0 );
            
            input_sync( g_evgen_dev );
        }
        
    } else {
        JSMAPPER_LOG_DEBUG( "sending key ID=%u, mod=0x%x, press=%i", key->id, key->modifiers, press );

        if( press || key->single ) 
            _send_key_modifiers( key->modifiers, 1 );
        
        input_event( g_evgen_dev, EV_KEY, key->id, press );
        
        if( press == 0 )
            _send_key_modifiers( key->modifiers, 0 );
        
        input_sync( g_evgen_dev );
    }
    
	return 0;
}


/****************************************************************************************************************
  
  Relative axes (AKA the mouse...)
  
****************************************************************************************************************/

static int send_rel_thread( void * data )
{
    struct send_rel_thread_params * rel = (struct send_rel_thread_params *) data;
    
    JSMAPPER_LOG_DEBUG( "started thread for relative axis ID=%u", rel->id );
    
    while( rel->cancel == 0 ) {
        JSMAPPER_LOG_DEBUG( "sending event for relative axis ID=%u, step=%i", rel->id, rel->step );
        input_event( g_evgen_dev, EV_REL, rel->id, rel->step );
        input_sync( g_evgen_dev );
        
        if( rel->spacing > 0 ) {
            msleep( rel->spacing );
        }
    }
    
    JSMAPPER_LOG_DEBUG( "finished thread for relative axis ID=%u", rel->id );
    
    /* destroy ptr to data: */
    kfree( rel );
    return 0;
}


static void send_rel_work( struct work_struct * work )
{
	struct send_rel_work_params		* w = (struct send_rel_work_params *) work;
	struct task_struct				* thread = NULL;
	struct send_rel_thread_params	* thread_p = NULL;

	spin_lock( &g_evgen_lock );
	
	if( g_evgen_rel[w->rel.id] ) {
        /* stop current thread, if any: */
        JSMAPPER_LOG_DEBUG( "stopping thread for relative axis ID=%u", w->rel.id );
        g_evgen_rel[w->rel.id]->cancel = 1;
        g_evgen_rel[w->rel.id] = NULL;
    }
    
    if( w->press ) {
        /* copy parameters: */
        thread_p = (struct send_rel_thread_params *) kzalloc( sizeof( struct send_rel_thread_params ), GFP_KERNEL );
        if( thread_p ) {
			memcpy( thread_p, &w->rel, sizeof( w->rel) );
            
            /* launch thread: */
            thread = kthread_create( send_rel_thread, thread_p, "rel_axis" );	// TODO add axis ID to thread name
            if( thread ) {
				g_evgen_rel[w->rel.id] = thread_p;
                wake_up_process( thread );
                
            } else {
                JSMAPPER_LOG_ERROR( "failed to launch relative axis thread!" );
                kfree( thread_p );
            }
            
        } else {
            JSMAPPER_LOG_ERROR( "failed to allocate parameters for relative axis thread!" );
        }
    }
	
	kfree( w );
	
	spin_unlock( &g_evgen_lock );
}


int jsmapper_evgen_send_rel( struct jsmapdev_core_rel * rel, int press )
{
    int							result = 0;
	struct send_rel_work_params	* w = NULL;
    
    
	if( g_evgen_dev == NULL ) {
        JSMAPPER_LOG_ERROR( "event generator device not available!" );
		return -ENODEV;
	}
	
    if( rel->id >= REL_MAX ) {
        JSMAPPER_LOG_ERROR( "invalid relative axis ID=%u!", rel->id );
		return -EINVAL;
    }

    if( rel->single ) {
        
        /* throw a single event on button press */
        if( press ) {
            JSMAPPER_LOG_DEBUG( "sending event for relative axis ID=%u, step=%i", rel->id, rel->step );
            input_event( g_evgen_dev, EV_REL, rel->id, rel->step );
            input_sync( g_evgen_dev );
        }
        
    } else {

		/* launch action thread: 
		 *	  As we are in IRQ context, we can't simply create a thread here (causes a kernel crash). Instead, 
		 * we defer the thread creation to a work item, which will get executed under normal process context.
	     */
        w = (struct send_rel_work_params *) kzalloc( sizeof( struct send_rel_work_params ), GFP_ATOMIC );
        if( w ) {
			/* copy parameters: */
			w->press = press;
			w->rel.id = rel->id;
            w->rel.step = rel->step;
            w->rel.spacing = rel->spacing;
			
			/* ok, queue work item: */
			INIT_WORK( &w->work, send_rel_work );
			result = queue_work( g_evgen_wq, &w->work );
                
        } else {
            JSMAPPER_LOG_ERROR( "failed to allocate work item!" );
            result = -ENOMEM;
        }
    }
	
	return result;
}



/****************************************************************************************************************
  
  Macros
  
****************************************************************************************************************/

/**
  * \brief Work function for key sequence
  * 
  * This callback function gets called from the workqueue when the new item gets scheduled. It takes caree
  * of actually sending te keys, then cleaning up the data.
  */

static void _send_macro( struct work_struct * work )
{
    struct send_macro_work * p = (struct send_macro_work *) work;
    struct jsmapdev_core_key * key = NULL;
    int i = 0;

    JSMAPPER_LOG_DEBUG( "sending macro (keys: %u)", p->macro.count );
    
    key = p->macro.keys;
    for( i = 0; i < p->macro.count; i++ ) {
        JSMAPPER_LOG_DEBUG( "sending macro key ID=%u, mod=0x%x", key->id, key->modifiers );
        
        _send_key_modifiers( key->modifiers, 1 );
        input_event( g_evgen_dev, EV_KEY, key->id, 1 );
        input_event( g_evgen_dev, EV_KEY, key->id, 0 );
        _send_key_modifiers( key->modifiers, 0 );
        
        input_sync( g_evgen_dev );
        
        if( i < p->macro.count - 1 ) {
            msleep( p->macro.spacing );
		}
        
        key++;
    }
    
    JSMAPPER_LOG_DEBUG( "finished sending macro." );
  
    kfree( p->macro.keys );
    kfree( p );
}


int jsmapper_evgen_send_macro( struct jsmapdev_core_macro * macro, int press )
{
    int result = 0;
	size_t keys_cb = 0;
    struct send_macro_work * p = NULL;
	
    if( g_evgen_dev == NULL ) {
        JSMAPPER_LOG_ERROR( "event generator device not available!" );
        return -ENODEV;
    }
    
    if( g_evgen_wq == NULL ) {
        JSMAPPER_LOG_ERROR( "workqueue not available!" );
        return -ENODEV;
    }
        
    
    if( press ) {
        /* create work item: */
        p = (struct send_macro_work *) kmalloc( sizeof( struct send_macro_work ), GFP_ATOMIC );
        if( p ) {
            keys_cb = sizeof( macro->keys[0] ) * macro->count;
            p->macro.keys = (struct jsmapdev_core_key *) kmalloc( keys_cb, GFP_ATOMIC );
            if( p->macro.keys ) {
                /* copy keys, etc...: */
                memcpy( p->macro.keys, macro->keys, keys_cb );
                p->macro.count = macro->count;
                p->macro.spacing = macro->spacing;
                
                /* ok, queue work item: */
                INIT_WORK( &p->work, _send_macro );
                result = queue_work( g_evgen_wq, &p->work );
                
            } else {
                JSMAPPER_LOG_ERROR( "failed to allocate keys buffer!" );
                kfree( p );
                result = -ENOMEM;
            }
        
        } else {
            JSMAPPER_LOG_ERROR( "failed to allocate work item!" );
            result = -ENOMEM;
        }
    
    } else {
        /* do nothing on button release */
    }

    return result;    
}

