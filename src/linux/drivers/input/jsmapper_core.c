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
 * \file jsmapper_core.c
 * \brief JSMapper kernel module mapping core
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#include "jsmapper_core.h"
#include "jsmapper_api.h"
#include "jsmapper_log.h"
#include "jsmapper_evgen.h"

#include <linux/kernel.h>
#include <linux/input.h>
#include <linux/slab.h>


/*******************************************************************************************************
 * 
 * Main core functions
 * 
 *******************************************************************************************************/

struct jsmapdev_core * jsmapper_core_init( struct input_dev * dev )
{
	struct jsmapdev_core * core = NULL;
	int i = 0;
	
	if( dev ) {
		core = kzalloc(sizeof(struct jsmapdev_core), GFP_KERNEL);
		if( core ) {
			core->dev = dev;
			core->profile_name = NULL;
			/* get buttons: 
			 * 	Buttons IDs are extracted using a double loop system, just as joydev.c does, so jsmapper
			 * will map them in the same way original joystick driver does.
			 */
			for (i = BTN_JOYSTICK - BTN_MISC; i < KEY_MAX - BTN_MISC + 1; i++) {
				if (test_bit(i + BTN_MISC, dev->keybit)) {
					core->button_map[i] = core->button_count;
					core->button_rmap[core->button_count++] = i + BTN_MISC;
				}
			}
			for (i = 0; i < BTN_JOYSTICK - BTN_MISC; i++) {
				if (test_bit(i + BTN_MISC, dev->keybit)) {
					core->button_map[i] = core->button_count;
					core->button_rmap[core->button_count++] = i + BTN_MISC;
				}
			}

			/* get axes */
			for (i = 0; i < ABS_CNT; i++) {
				if (test_bit(i, dev->absbit)) {

					JSMAPPER_LOG_DEBUG( "Detected axis ID=%i, minValue=%i, maxValue=%i => %i",
										(int) i,
										(int) input_abs_get_min( dev, i ),
										(int) input_abs_get_max( dev, i ),
										(int) core->axis_count );

					core->axis_map[i] = core->axis_count;
					core->axis_rmap[core->axis_count++] = i;
				}
			}
			for( i = 0; i < core->axis_count; i++ ) {
				core->current_axis_action[ i ] = NULL;
			}

			
			/* initialize mode structure */
			core->root_mode = jsmapper_core_init_mode( core );
            core->last_mode_id = 0;
			
		} else
			JSMAPPER_LOG_ERROR( "unable to allocate a core structure!" );
	}
	
	return core;
}


void jsmapper_core_clear( struct jsmapdev_core * core, int reset )
{
	int i = 0;

	if( core ) {
		
		/* clear profile name */
		if( core->profile_name ) {
			kfree( core->profile_name );
			core->profile_name = NULL;
		}

		/* TODO terminate any current action...? */
		for( i = 0; i < core->axis_count; i++ ) {
			if( core->current_axis_action[ i ] ) {
				JSMAPPER_LOG_DEBUG( "Deactivating old action for axis ID=%u on cleanup", i );
				jsmapper_evgen_send_action( &core->current_axis_action[ i ]->action, 0 );
			}
			
			core->current_axis_action[ i ] = NULL;
		}

		/* destroy root mode, including old all submodes: */
		if( core->root_mode ) {
			jsmapper_core_clear_mode( core, core->root_mode );
			core->root_mode = NULL;
		}
		
		core->last_mode_id = 0;

		JSMAPPER_LOG_INFO( "device cleared" );

		if( reset ) {
			/* re-create new root mode: */
			core->root_mode = jsmapper_core_init_mode( core );
		}
	}
}


void jsmapper_core_done( struct jsmapdev_core * core )
{
	if( core ) {
		jsmapper_core_clear( core, 0 );
		kfree( core );
	}
}


void jsmapper_core_set_profile_name( struct jsmapdev_core * core, char * profile_name )
{
	if( core->profile_name && core->profile_name != profile_name ) {
		kfree( core->profile_name );
	}

	core->profile_name = profile_name;
}

char * jsmapper_core_get_profile_name( struct jsmapdev_core * core )
{
	return core->profile_name;
}


/********************************************************************************************************
 * 
 * Mode management functions
 * 
 ********************************************************************************************************/

int jsmapper_core_add_mode( struct jsmapdev_core * core, struct t_JSMAPPER_MODE * mode_p )
{
    int ret = -EINVAL;
    
    if( core && mode_p ) {
        struct jsmapdev_core_mode * parent_mode = jsmapper_core_find_mode( core->root_mode, mode_p->parent_mode_id );
        if( parent_mode ) {
			struct jsmapdev_core_mode * mode = jsmapper_core_init_mode( core );
            if( mode ) {
                
                mode->mode_id = ++core->last_mode_id;
                mode->parent = parent_mode;
                list_add_tail( &mode->child_item, &parent_mode->children_list );
                
                mode->condition_type = mode_p->condition_type;
                if( mode->condition_type == JSMAPPER_MODE_CONDITION_BUTTON ) {
					mode->condition.button.id = mode_p->condition.button.id;
                } else if( mode->condition_type == JSMAPPER_MODE_CONDITION_AXIS ) {
					mode->condition.axis.id = mode_p->condition.axis.id;
					mode->condition.axis.low = mode_p->condition.axis.low;
					mode->condition.axis.high = mode_p->condition.axis.high;
                }
                
                JSMAPPER_LOG_INFO( "created new mode ID=%u", (uint) mode->mode_id );
                mode_p->mode_id = mode->mode_id;
                ret = 0;
                        
            } else {
                JSMAPPER_LOG_ERROR( "failed to allocate new mode!" );
                ret = -ENOMEM;
            }
            
        } else {
            JSMAPPER_LOG_ERROR( "invalid parent mode ID: %u", (uint) mode_p->parent_mode_id );
        }
    }
    
    return ret;
}


struct jsmapdev_core_mode * jsmapper_core_init_mode( struct jsmapdev_core * core )
{
	int i = 0;
	
	struct jsmapdev_core_mode * mode = kzalloc(sizeof(struct jsmapdev_core_mode), GFP_KERNEL);
	if( mode ) {
        mode->parent = NULL;
        mode->mode_id = 0;
        mode->condition_type = JSMAPPER_MODE_CONDITION_NONE;
        memset( &mode->condition, 0, sizeof( mode->condition ) );
        INIT_LIST_HEAD( &mode->child_item );
        INIT_LIST_HEAD( &mode->children_list );
        
		// buttons array
		mode->buttons = kzalloc(sizeof(struct jsmapdev_core_button_action) * core->button_count, GFP_KERNEL);
		if( mode->buttons ) {
			for( i = 0; i < core->button_count; i++ ) {
				mode->buttons[i].filter = false;
				jsmapper_core_init_action( &mode->buttons[i].action );
			}
		}
		else
			JSMAPPER_LOG_ERROR( "unable to allocate %u-button array!", core->button_count );
		
		// axes array:
		mode->axes = kzalloc(sizeof(struct jsmapdev_core_axis_actions) * core->axis_count, GFP_KERNEL);
		if( mode->axes ) {
			for( i = 0; i < core->axis_count; i++ ) {
				jsmapper_core_init_axis_actions( &mode->axes[i] );
			}
		}
		else
			JSMAPPER_LOG_ERROR( "unable to allocate %u-axis array!", core->axis_count );
	}
	else
		JSMAPPER_LOG_ERROR( "unable to allocate a mode structure!" );
	
	return mode;
}


struct jsmapdev_core_mode * jsmapper_core_find_mode( struct jsmapdev_core_mode * root, uint mode_id )
{
    struct jsmapdev_core_mode * result = NULL;
    struct jsmapdev_core_mode * child = NULL;

    if( root ) {
        if( root->mode_id == mode_id ) {
            result = root;
        } else {
            /** start recursive search */
            list_for_each_entry( child, &root->children_list, child_item ) {
                result = jsmapper_core_find_mode( child, mode_id );
                if( result )
                    break;
            }
        }
    }

    return result;
}


int jsmapper_core_mode_is_active( struct jsmapdev_core * core, struct jsmapdev_core_mode * mode )
{
	int result = 0;
	
	switch( mode->condition_type ) {
	case JSMAPPER_MODE_CONDITION_BUTTON:
		if( mode->condition.button.id >= 0 && mode->condition.button.id < core->button_count ) {
			uint key = core->button_rmap[ mode->condition.button.id ];
			if( test_bit( key, core->dev->key) ) {
				result = 1;
			}
		}
		break;
	
	case JSMAPPER_MODE_CONDITION_AXIS:
		if( mode->condition.axis.id >= 0 && mode->condition.axis.id < core->axis_count ) {
			uint axis = core->axis_rmap[ mode->condition.axis.id ];
			uint value = input_abs_get_val( core->dev, axis );
			if( value >= mode->condition.axis.low && value <= mode->condition.axis.high ) {
				result = 1;
			}
		}
		break;
	}
	
	return result;
}


void jsmapper_core_clear_mode( struct jsmapdev_core * core, struct jsmapdev_core_mode * mode )
{
    struct jsmapdev_core_mode * child = NULL;
    struct jsmapdev_core_mode * tmp = NULL;
            
	if( mode ) {
        JSMAPPER_LOG_INFO( "destroying mode ID=%u", (uint) mode->mode_id );
        
        list_for_each_entry_safe( child, tmp, &mode->children_list, child_item ) {
            list_del( &child->child_item );
			jsmapper_core_clear_mode( core, child );
        }
        
		if( mode->buttons ) {
			int i;
			for( i = 0; i < core->button_count; i++ )
				jsmapper_core_clear_action( &mode->buttons[i].action );
			
			kfree( mode->buttons );
		}
		
		if( mode->axes ) {
			int i;
			for( i = 0; i < core->axis_count; i++ )
				jsmapper_core_clear_axis_actions( &mode->axes[i] );
			
			kfree( mode->axes );
		}
		
		kfree( mode );
	}
}



/********************************************************************************************************
 *
 * Action functions
 *
 ********************************************************************************************************/

void jsmapper_core_init_action( struct jsmapdev_core_action * action )
{
	memset( action, 0, sizeof(struct jsmapdev_core_action) );
	action->type = JSMAPPER_ACTION_DEFAULT;
}

int jsmapper_core_copy_action( const struct jsmapdev_core_action * from,  struct jsmapdev_core_action * to )
{
    int result = 0;
    
    jsmapper_core_clear_action( to );

	to->type = from->type;
	switch( to->type )
	{
	case JSMAPPER_ACTION_NONE:
		break;
		
	case JSMAPPER_ACTION_KEY:
		to->key = from->key;
		break;
	
    case JSMAPPER_ACTION_REL:
        to->rel = from->rel;
        break;
        
	case JSMAPPER_ACTION_MACRO:
        to->macro.spacing = from->macro.spacing;
        if( from->macro.count > 0 ) {
            size_t cbKeys = sizeof( from->macro.keys[0] ) * from->macro.count;
            to->macro.keys = (struct jsmapdev_core_key *) kmalloc( cbKeys, GFP_KERNEL );
            if( to->macro.keys ) {
                memcpy( to->macro.keys, from->macro.keys, cbKeys );
                to->macro.count = from->macro.count;
            } else {
                JSMAPPER_LOG_ERROR( "failed to allocate memory for %u keys!", from->macro.count );
                result = -ENOMEM;
            }
        }
        break;
	
	default:
		break;
	}

	return 0;
}

void jsmapper_core_clear_action( struct jsmapdev_core_action * action )
{
    switch( action->type )
    {
    case JSMAPPER_ACTION_MACRO:
        if( action->macro.count > 0 && action->macro.keys ) {
            kfree( action->macro.keys );
            action->macro.keys = NULL;
            action->macro.count = 0;
        }
        break;
    
    default:
        break;
    }

    memset( action, 0, sizeof(struct jsmapdev_core_action) );
	action->type = JSMAPPER_ACTION_DEFAULT;
}


/********************************************************************************************************
 * 
 * Button action functions
 * 
 ********************************************************************************************************/

int jsmapper_core_set_button_action( struct jsmapdev_core * core, uint button_id, uint mode_id, const struct jsmapdev_core_button_action * assign )
{
	int ret = 0;
	struct jsmapdev_core_mode * mode = NULL;
	
	if( core 
			&& button_id >= 0
			&& button_id < core->button_count ) {
		
        mode = jsmapper_core_find_mode( core->root_mode, mode_id );
		if( mode && mode->buttons ) {
			mode->buttons[button_id].filter = assign->filter;
			ret = jsmapper_core_copy_action( &assign->action, &mode->buttons[button_id].action );
            if( ret == 0 )
				JSMAPPER_LOG_INFO( "assigned action to button ID=%u on mode ID=%u", button_id, mode_id );

		} else {
            JSMAPPER_LOG_ERROR( "invalid mode specified ID=%u", mode_id );
            ret = -EINVAL;
        }
    } else {
		JSMAPPER_LOG_ERROR( "invalid button ID=%u", button_id );
        ret = -EINVAL;
    }
	
	return ret;
}


int jsmapper_core_get_button_action( struct jsmapdev_core * core, uint button_id, uint mode_id, struct jsmapdev_core_button_action * assign )
{
	int ret = 0;
	struct jsmapdev_core_mode * mode = NULL;
	
	if( core 
			&& button_id >= 0
			&& button_id < core->button_count ) {
		
        mode = jsmapper_core_find_mode( core->root_mode, mode_id );
		if( mode && mode->buttons ) {
			assign->filter = mode->buttons[button_id].filter;
			ret = jsmapper_core_copy_action( &mode->buttons[button_id].action, &assign->action );
		} else {
            JSMAPPER_LOG_ERROR( "invalid mode specified ID=%u", mode_id );
            ret = -EINVAL;
        }
	} else {
		JSMAPPER_LOG_ERROR( "invalid button ID=%u", button_id );
		ret = -EINVAL;
    }
	
	return ret;
}


struct jsmapdev_core_button_action * jsmapper_core_find_button_action( struct jsmapdev_core * core, 
                                                                         struct jsmapdev_core_mode * mode, 
                                                                         int button_id )
{
	struct jsmapdev_core_button_action 	* action = NULL;
	struct jsmapdev_core_mode			* submode = NULL;
	
	if( core 
			&& button_id >= 0
			&& button_id < core->button_count) {
		
		/* first, try if mapped by any of the submodes: */
		list_for_each_entry_reverse( submode, &mode->children_list, child_item ) {
			if( jsmapper_core_mode_is_active( core, submode )) {
				action = jsmapper_core_find_button_action( core, submode, button_id );
				if( action != NULL ) {
					break;
				}
			}
		}
		
		/* if not, check if mapped by this mode: */
		if( action == NULL ) {
			if( mode->buttons ) {
				if( mode->buttons[ button_id ].action.type != JSMAPPER_ACTION_DEFAULT ) {
					// JSMAPPER_LOG_DEBUG( "    Found action in mode ID=%u for button ID=%i", mode->mode_id, button_id );
					action = &mode->buttons[ button_id ];
				}
			}
		}
	}
	
	return action;
}





/********************************************************************************************************
 * 
 * Axis action functions
 * 
 ********************************************************************************************************/

void jsmapper_core_init_axis_actions( struct jsmapdev_core_axis_actions * actions )
{
	INIT_LIST_HEAD( &actions->action_list );
}

void jsmapper_core_clear_axis_actions( struct jsmapdev_core_axis_actions * actions )
{
	struct jsmapdev_core_axis_action * assign = NULL;
    struct jsmapdev_core_axis_action * tmp = NULL;
	
	list_for_each_entry_safe( assign, tmp, &actions->action_list, child_item ) {
		list_del( &assign->child_item );
		jsmapper_core_clear_action( &assign->action );
		kfree( assign );
    }
}


int jsmapper_core_set_axis_action( struct jsmapdev_core * core, uint axis_id, uint mode_id, const struct jsmapdev_core_axis_action * assign )
{
	int ret = 0;
	struct jsmapdev_core_mode * mode = NULL;

	if( core
			&& axis_id >= 0
			&& axis_id < core->axis_count ) {

		/* find mode */
		mode = jsmapper_core_find_mode( core->root_mode, mode_id );
		if( mode && mode->axes ) {
			struct jsmapdev_core_axis_action * axis_assign		= NULL;
			struct jsmapdev_core_axis_actions * axis_actions	= &mode->axes[ axis_id ];

			/* check if an action featuring the same band is yet defined */
			list_for_each_entry( axis_assign, &axis_actions->action_list, child_item ) {
				if( axis_assign->band_high == assign->band_high
						&& axis_assign->band_low == assign->band_low ) {
					axis_assign->band_high = assign->band_high;
					axis_assign->band_low = assign->band_low;
					axis_assign->filter = assign->filter;
					jsmapper_core_copy_action( &assign->action, &axis_assign->action );

					JSMAPPER_LOG_INFO( "modified action on axis ID=%u, band=(%i,%i) on mode ID=%u",
									   axis_id, assign->band_low, assign->band_high, mode_id );

					assign = NULL;	/* done */
					break;
				}
			}

			/* if not done yet, then add it to the list */
			if( assign ) {
				axis_assign = kzalloc( sizeof(struct jsmapdev_core_axis_action), GFP_KERNEL);
				if( axis_assign ) {
					axis_assign->band_high = assign->band_high;
					axis_assign->band_low = assign->band_low;
					axis_assign->filter = assign->filter;
					jsmapper_core_init_action( &axis_assign->action );
					jsmapper_core_copy_action( &assign->action, &axis_assign->action );

					INIT_LIST_HEAD( &axis_assign->child_item );
					list_add( &axis_assign->child_item, &axis_actions->action_list );

					JSMAPPER_LOG_INFO( "assigned action on axis ID=%u, band=(%i,%i) on mode ID=%u",
									   axis_id, assign->band_low, assign->band_high, mode_id );
				} else {
					JSMAPPER_LOG_ERROR( "failed to allocate new axis action!" );
					ret = -ENOMEM;
				}
			}

		} else {
			JSMAPPER_LOG_ERROR( "invalid mode ID=%u", mode_id );
			ret = -EINVAL;
		}

	} else {
		JSMAPPER_LOG_ERROR( "invalid axis ID=%u", axis_id );
		ret = -EINVAL;
	}

	return ret;
}


struct jsmapdev_core_axis_action * jsmapper_core_find_axis_action( struct jsmapdev_core * core, 
                                                                   struct jsmapdev_core_mode * mode, 
                                                                   int axis_id, int value )
{
	struct jsmapdev_core_mode			* submode = NULL;
	struct jsmapdev_core_axis_actions	* axis_actions = NULL;
	struct jsmapdev_core_axis_action 	* axis_action = NULL;
	struct jsmapdev_core_axis_action 	* action = NULL;
	
	if( core 
			&& axis_id >= 0
			&& axis_id < core->axis_count) {
		
		/* first, try if mapped by any of the submodes: */
		list_for_each_entry_reverse( submode, &mode->children_list, child_item ) {
			if( jsmapper_core_mode_is_active( core, submode )) {
				action = jsmapper_core_find_axis_action( core, submode, axis_id, value );
				if( action != NULL )
					break;
			}
		}
		
		/* if not, check if mapped by this mode: */
		if( action == NULL ) {
		
			if( mode->axes ) {
				// JSMAPPER_LOG_DEBUG( "Checking mode ID=%u for action on axis ID=%u, value=%i", mode->mode_id, button_id, value );
				axis_actions = &mode->axes[ axis_id ];
	
				/* check if an action featuring the same band is yet defined */
				list_for_each_entry_reverse( axis_action, &axis_actions->action_list, child_item ) {
					if( value >= axis_action->band_low 
							&& value <= axis_action->band_high 
					        && axis_action->action.type != JSMAPPER_ACTION_DEFAULT ) {
						
						// JSMAPPER_LOG_DEBUG( "Found action in mode ID=%u for axis ID=%i, value=%i", mode->mode_id, axis_id, value );
						action = axis_action;
						break;
					}
				}
			}
		}
	}
	
	return action;
}



/********************************************************************************************************
 * 
 * Helper functions
 * 
 ********************************************************************************************************/

int jsmapper_core_map_button( struct jsmapdev_core * core, int code )
{
	int buttonId = -1;
	
	if( code >= BTN_MISC && code < KEY_MAX ) {
		buttonId = core->button_map[code - BTN_MISC];
	}
	
	return buttonId;
}

int jsmapper_core_rmap_button( struct jsmapdev_core * core, int code )
{
	int buttonId = -1;
	
	if( code >= 0 && code < core->button_count ) {
		buttonId = core->button_rmap[code];
	}
	
	return buttonId;
}

int jsmapper_core_map_axis( struct jsmapdev_core * core, int code )
{
	int axisID = -1;

	if( code >= 0 && code < ABS_CNT ) {
		axisID = core->axis_map[ code ];
	}

	return axisID;
}

int jsmapper_core_rmap_axis( struct jsmapdev_core * core, int code )
{
	int axisId = -1;
	
	if( code >= 0 && code < core->axis_count ) {
		axisId = core->axis_rmap[code];
	}
	
	return axisId;
}
