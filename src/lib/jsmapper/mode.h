/**
 * Copyright 2013 Eduard Huguet Cuadrench
 * 
 * This file is part of JSMapper Library.
 * 
 * JSMapper Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License.
 * 
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with JSMapper Library.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * \file mode.h
 * \brief Declaration file for JSMapper mode class
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#ifndef __JSMAPPERLIB_MODE_H_
#define __JSMAPPERLIB_MODE_H_

#include "common.h"
#include <string>
#include <vector>

namespace jsmapper
{
	/**
	 * \brief Mode class
	 * 
	 * This class represents an operational mode for the device: this is, a set of assignments to buttons, 
	 * axes changing conditions, etc... which get applied whenever the mode is activated. The mode, thus, 
	 * stores the mapping between source event inputs (buttons, axes) and output actions.
	 * 
	 * Both axis and buttons are referenced using their name, which is then mapped to actual element ID 
     * at device loading time.
	 *
	 * A mode can itself have child submodes, which might override any of the assignments made by parent mode.
	 */
	class Mode
	{
	public:
		/**
		 * \brief Initializes the class
		 * 
		 * Initializes the class, using the given attributes
		 * 
		 * \param profile Pointer to containing profile. Needed for action name resolving.
         * \param parent Pointer to parent mode, or else NULL if this is the root mode
		 * \param cond Pointer to activation condition, or else NULL if this is the root mode
		 */
		Mode( Profile * profile, Mode * parent = NULL, Condition * cond = NULL );
		virtual ~Mode();
		
	
	// basic attributes
	public:
		/**
		 * \brief Clears mode data
		 * 
		 * Clears mode data: name, description, condition, etc...
		 */
		void clear();
		
		/**
		 * \brief Returns current mode's activation condition
		 */
		Condition * getCondition() const;
		
		/**
		 * \brief Sets mode's activation condition
		 */
		void setCondition( Condition * cond );
		
		
		/**
		 * \brief Return mode's name
		 */
		const std::string & getName() const;
		
		/**
		 * \brief Changes action's name
		 * 
		 * The mode name is for identification purposes only, as it's not entered into the device later when 
		 * programming it.
		 */
		void setName( const std::string &name );
		
		
		/**
		 * \brief Return action's description
		 */
		const std::string & getDescription() const;
		
		/**
		 * \brief Changes action's description
		 * 
		 * The mode description is for identification purposes only, as it's not entered into the device later when 
		 * programming it.
		 */
		void setDescription( const std::string &description );

		
	// relationships
	public:
		/**
		 * \brief Returns parent mode
		 * 
		 * If this is the root mode, then it won't have a parent mode, so this function will return NULL.
		 */
		Mode * getParent() const;

		/**
		 * \brief Changes parent mode
		 */
		void setParent( Mode * parent );
		
		/**
		 * \brief Adds a child mode to this one
		 * 
		 * Child modes will be destroyed when the class is deleted.
		 */
		void addChild( Mode * mode );
		
		/**
		 * \brief Returns current children submodes
		 */
		const ModeList &getChildren() const;
		
		/**
		 * \brief Removes the child mode from this parent
		 * 
		 * The mode is not deleted after calling this function.
		 */
		void removeChild( Mode * mode );
		
		
	
	// button action mapping:
	public:
		/**
		 * \brief Clears all button assignments
		 */
		void clearButtons();
		
		/**
		 * \brief Assigns an action to the given button
		 * 
		 * This function assigns the given action to the given button ID. If an empty string is given, then it will
		 * clear previous assignment.
		 * 
		 * \param id Button name ID
		 * \param action Name of the new action to assign, or an empty string to clear previous one.
		 *
		 * The action should hav been registered first into the profile by calling addAction()
		 *
		 */
		void setButtonAction( const std::string &id, const std::string &action );
		

		/**
		 * \brief Returns currently assigned action to given button
		 * 
		 * \param id Button name ID
		 * \return Action name assigned, or an empty string if none
		 */
		std::string getButtonAction( const std::string &id ) const;
		
		
		/**
		 * \brief Returns the list of mapped buttons
		 * 
		 * This function returns the list of buttons which are currenrly mapped to any action.
		 * 
		 * \return A list of Button name IDs
		 */
		std::vector<std::string> getButtons() const;
		

    // axis action mapping:
    public:
        /**
         * \brief Clears all axes assignments
         */
        void clearAxes();

        /**
         * \brief Assigns an action to the given axis
         *
         * This function assigns the given action to the given axis, identified by its name. The action
         * will be triggered when user operation makes axis value enter the designated band range.
         *
         * If an empty action name is given, then it will clear previous assignment.
         *
         * \param id Axis name ID
         * \param band Axis band to assign an action to
         * \param action Name of the new action to assign, or an empty string to clear previous one.
         *
         * The action should hav been registered first into the profile by calling addAction()
         *
         */
        void setAxisAction( const std::string &id, const Band &band, const std::string &action );


        /**
         * \brief Returns currently assigned action to axis for given band
         *
         * \param id Axis name ID
         * \param band Axis band to retrieve action for
         * \return Action name assigned, or an empty string if none
         */
        std::string getAxisAction( const std::string &id, const Band &band ) const;


        /**
         * \brief Returns the list of mapped axes
         *
         * This function returns the list of axes which are currenrly mapped to any action.
         *
         * \return A list of axes name IDs
         */
        std::vector<std::string> getAxes() const;

        /**
         * \brief Returns defined axis bands
         *
         * Returns the list of bands defined for a given axis
         *
         * \param id Axis name ID
         * \return A list of bands
         */
        std::vector<Band> getAxisBands( const std::string &id ) const;


	// serialization:
	public:
		/**
		 * \brief Saves mode to an XML node and returns it
		 */
		virtual xmlNodePtr toXml() const;
		
		/**
		 * \brief Restores mode from an XML node
		 */
		virtual bool fromXml( xmlNodePtr  node );


        
    // loading into device:d
    public:
        /**
         * \brief Loads profile into device
         * 
         * This function will load all profile mappings into the device. It will clear the device first, then load 
         * all the modes, mappings, etc... into the device.
         */
        bool toDevice( Device * dev );
        
        /**
          \brief Returns mode ID inside device
          
          This function is used only during module loading into device. It will return the identifier the mode got 
          assigned when it was loaded into the device, so child modes can refer to it.
          */
        uint getModeId() const;


	protected:
		/**
		 * \brief Loads button assignments into device
		 */
		bool buttonsToDevice( Device * dev );

		/**
		 * \brief Loads axes assignments into device
		 */
		bool axesToDevice( Device * dev );


	private:
		class Private;
		Private * d;
	};
}

#endif

