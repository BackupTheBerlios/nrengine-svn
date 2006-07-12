/***************************************************************************
 *                                                                         *
 *   (c) Art Tevs, MPI Informatik Saarbruecken                             *
 *       mailto: <tevs@mpi-sb.mpg.de>                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef _NR_NAME_VALUE_PAIRS_H_
#define _NR_NAME_VALUE_PAIRS_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "Dator.h"
#include <boost/tokenizer.hpp>


namespace nrEngine{
	
	//! Name to value pair list derived from STL's classes
	/**
	* This class is used to specify parameters by string pairs. Each function which get
	* an object of such class should know how to interpret them. Each element of the
	* list is a pair of strings. So it looks like name1=value1, name2=value2, ...
	*
	* New elements can easily be added by provided functions.
	*
	* \ingroup gp
	**/
	class _NRExport NameValuePairs: public std::map<std::string, std::string> {
	public:
	
		/**
		* Add new element to the list, by using simpler function.
		* Each element is a pair of a name and a value.
		* @param name Name for the variable
		* @param value Value for the variable "name"
		**/
		void add(const std::string& name, const std::string& value){
			(*this)[name] = value;
		}
		
		/**
		 * Get a value associated to the specified index. The index
		 * defines a position in the map. If the map contains no elements
		 * so empty string is returned
		 **/
		std::string get(const uint32 index) const {
			if (index >= size()) return std::string();
			
			NameValuePairs::const_iterator it = begin();
			for (uint32 i = 0; i < index; i++) it ++;
			assert(it != end());			
			return it->second;
		}

		/**
		 * Set the value by it's index
		 **/
		void set(uint32 index, const std::string& value){
			if (index >= size()) return;
			
			NameValuePairs::iterator it = begin();
			for (uint32 i = 0; i < index; i++) it ++;

			assert(it != end());

			it->second = value;
		}
				
		/**
		* Return the value according to the name. If no such element found
		* empty string will be returned.
		* @param name Name of the variable
		**/
		std::string get(const std::string& name) const{
			NameValuePairs::const_iterator it = find(name);
			if (it != end()){
				return it->second;
			}
			return std::string();
		}
	
		/**
		 * Check whenever a certain key exists
		 **/
		bool exists(const std::string& name) const{
			NameValuePairs::const_iterator it = find(name);
			
			return it != end();
		}
		
		/**
		* Assign the value found by the given name to the given dator.
		*
		* @param name Name of the varibale containing in the name value pair list
		* @param dator Dator to which one the new value will be assigned, if such one is found
		* @return either OK or:
		*				- BAD_PARAMETERS if such a variable was not found
		*				- NOT_VALID_DATOR If the given dator does not support multiple set values and the variable is a multiple set.
		*								In this case only the first element will be assigned.
		**/
		Result assign(const std::string& name, IDator& dator){
			const std::string& str = get(name);
			if (str.length() == 0){
				return BAD_PARAMETERS;
			}
			
			// now tokenize the value to allow multisets as values
			boost::char_separator<char> sep(", ");
			boost::tokenizer<boost::char_separator<char> > tok(str, sep);
			boost::tokenizer<boost::char_separator<char> >::iterator it = tok.begin();
			
			// the first token will be asigned directly
			dator = *it;
			
			// all other tokens will be assigned only if we have such tokens and the given dator
			// does support multiple values
			for(; it != tok.end(); it++){
				// now check if the dator support multiple set values
				if (!dator.hasMultipleValues()) return NOT_VALID_DATOR;
				
				// add new element into multiple set of elements
				if (it->length() > 0){
					dator += *it;
				}
			}
			
			return OK;
		}
 	
 	
	};

}; // end namespace

#endif
