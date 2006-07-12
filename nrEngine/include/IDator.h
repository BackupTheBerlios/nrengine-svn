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


#ifndef _NR_C_DATOR__H_
#define _NR_C_DATOR__H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"


namespace nrEngine{	
	
	//! Parser for value string used by dator for parsing
	/**
	* This class is used to interpret the value of the given string.
	* If you want to store something special as standard values in a dator,
	* you have to know how to interpretate them. For example you want to store
	* something like "name_A = ONE_VALUE | TWO_VALUES" instead of "name_A = 32".
	* The first case does explain itself what does the values means. In the second
	* case it is not clear what does 32 means here. So this interpreter can help you
	* to find out what does values means.
	*
	* You have to derive your own value interpreters to use in your application. This is because
	* we can not know what kind of interpreters do you need.
	*
	* \ingroup gp
	**/
	template <class T> class _NRExport CValueParser{
	public:
	
		/** Construcotr of the value parser object **/
		CValueParser(){}

		/** Destructor **/
		virtual ~CValueParser(){}
		
		/**
		* Parse given string to a value of type <b>T</b>
		**/
		virtual T parse(const std::string& str){
			return T();
		}
 	
 	
		/**
		* Just convert a value of type T into a string.
		**/
		virtual std::string convert(const T& value){
			return std::string();
		}
 	
	};
	
	
	//! Same as functor but for data objects
	/**
	* IDator - is an interface for our templated class to hold all kind of data.
	* Dator is the same like Functor for functions. Dator can store data with
	* different types (int, float, multiple value, ...). New values can be assigned
	* through the same dator object or through std::string values (useful for scripting)
	* \ingroup gp
	**/
	class _NRExport IDator{
	public:
	
		IDator(){}
		virtual ~IDator(){}
			
		//! Sign a string to this dator
		virtual IDator&	operator =	(const std::string &s) { return *this; }
		
		//! Add new value in form of string to the dator
		virtual IDator&	operator +=	(const std::string &s) { return *this; }
		
		//! Substract new value in form of string from the dator
		virtual IDator&	operator -=	(const std::string &s) { return *this; }
		
		//! Check whenever two dators are the same
		virtual bool 		operator ==	(const std::string &s) { return *this; }
		
		//! Check whenever two dators are not the same
		virtual bool 		operator !=	(const std::string &s) { return *this; }
	
	
		virtual operator 	std::string() 	= 0;
		virtual operator 	int() 			= 0;
		virtual operator 	bool() 			= 0;
		virtual operator 	float() 		= 0;
		virtual const char* str() 			= 0;
		
		virtual bool hasMultipleValues() = 0;
		
		
	};
	
	
	
	//! Derived templated dator class
	/**
	* CDator - is a derived class from IDator representing storage for all kind
	* of data. This class is templated so you can use it to store data of any type.
	* To convert stored data to std::string @a ::boost::lexical_cast<std::string>(T) is used.
	* Some of functions are defined as virtual, so you can override this
	* to implement dator for non standard types.
	*
	* <b>value_parser</b> is used to parse a value string if ::boost's lexical_cast fails.
	* Define your own parser to parse values like <i>"VALUE_ONE"</i> to <i>"1"</i> or similar.
	* \ingroup gp
	**/
	template<class T, class value_parser = CValueParser<T> >
	class _NRExport CDator : public IDator{
	protected:
		
		//! Value/Target to be stored in
		T& target;
		
		//! Smart pointer to a value parser
		SharedPtr< value_parser > valParser;
		
		
		/**
		* Convert given string to value of type T
		**/
		virtual T toVal(const std::string &s){
			
			try{
				return (boost::lexical_cast<T>(s));
			}catch (boost::bad_lexical_cast &expr){
				if (valParser != NULL){
					return valParser->parse(s);
				}
				// We do not know what to return, so throw an error
				throw (expr);
			}
		}
		
		
		/**
		* Convert given value to a string
		**/
		virtual std::string toString(const T &val){
			
			try{
				return boost::lexical_cast<std::string>(val);
			}catch (boost::bad_lexical_cast &expr){
				if (valParser != NULL){
					return valParser->convert(val);
				}
				// We do not know what to return, so throw an error
				throw (expr);
			}		
		}
		
		
	public:
		
		/**
		* Create a dator from a variable. You can also specify value parser which will be used to parse
		* values that can not be casted lexicaly by ::boost
		* @param t Variable
		* @param parser Smart pointer to a value parser
		**/
		CDator(T& t, SharedPtr<value_parser> parser = SharedPtr<value_parser>()):target(t), valParser(parser) {}
		
		
		/**
		* Copy constructor for the dator.
		**/
		CDator(const IDator &s) {
			// is not so pretty, but works. TODO: rewrite it
			if (s.hasMultipleValues()){
				valParser = (static_cast<CDatorList<T, value_parser> >(s)).valParser;
			}else{
				valParser = (static_cast<CDator<T, value_parser> >(s)).valParser;
			}
			
			target = s;//toVal(std::string(s));
		}
		
		/**
		* Get the value of stored variables. The returned value has type T
		**/
		T get(){
			return target;
		}
		
		
		operator std::string() 		{ return toString(target); }
		const char* str() 			{ return toString(target).c_str();}
		virtual operator int() 		{ return static_cast<int>(target); }
		virtual operator bool() 	{ return static_cast<bool>(target); }
		virtual operator float() 	{ return static_cast<float>(target); }
		
		// operators to handle with strings
		IDator &operator 	= (const std::string &s) 	{ target=toVal(s); return *this; }
		IDator &operator 	+=(const std::string &s) 	{ target+=toVal(s); return *this; }
		IDator &operator 	-=(const std::string &s) 	{ target-=toVal(s); return *this; }
		bool 	operator 	==(const std::string &s) 	{ return (s==(std::string)(*this)); }
		bool 	operator 	!=(const std::string &s) 	{ return (s!=(std::string)(*this)); }
	
		// operators to other IDator objects
		IDator &operator 	= (const T &s)		{target=s; return *this;}
		IDator &operator 	+=(const T &s)		{target+=s; return *this;}
		IDator &operator 	-=(const T &s)		{target-=s; return *this;}
		bool 	operator	==(const T &s)		{return (target == s);}
		bool 	operator	!=(const T &s)		{return (target != s);}
		
		// other methods
		bool hasMultipleValues() 			{ return false; }
	
	};
	
	//! Create dators for variables with type string
	/**
	* This dator is written special for the case, that you want to have a dator
	* which should also handle strings.
	* \ingroup gp
	**/
	class _NRExport CStringDator : public IDator{
	protected:
		
		//! Value/Target to be stored in
		std::string& target;
	
		/**
		* Convert given string to value of type T
		**/
		std::string toVal(const std::string &s){
			return s;		
		}
		
		/**
		* Convert given value to a string
		**/
		std::string toString(const std::string &val){
			return val;
		}
	
	public:
		
		/**
		* Create a dator from a variable. No parser could be specified because string is string
		* @param t Variable
		**/
		CStringDator(std::string& t):target(t){}
		
		
		/**
		* Copy constructor for the dator.
		**/
		CStringDator(const CStringDator &s) : target(s.get()) {
		}
		
		/**
		* Get the value of stored variables. The returned value has type T
		**/
		std::string& get() const {
			return target;
		}
			
		operator std::string() 	{ return target; }
		const char* str() 		{ return target.c_str();}
		operator int() 			{ throw boost::bad_lexical_cast(); }
		operator bool() 		{ throw boost::bad_lexical_cast(); }
		operator float() 		{ throw boost::bad_lexical_cast(); }
		
		// operators to handle with strings
		IDator &operator	= (const std::string &s)		{ target = s; return *this;}
		IDator &operator 	+=(const std::string &s) 		{ target+=s; return *this; }
		IDator &operator 	-=(const std::string &s) 		{ return *this; }
		bool 	operator 	==(const std::string &s) 		{ return (s==(std::string)(*this)); }
		bool 	operator 	!=(const std::string &s) 		{ return (s!=(std::string)(*this)); }
	
		// other methods
		bool hasMultipleValues() 			{ return false; }
	
	};
	
	
	//! Same as dator but for list types containing elements of the same type
	/**
	* CDatorList - same like @a CDator but it can contain a multiple data type.
	* That means you can store more elements of the same type in a one Dator.
	* This can be used to implement Array/Multiple Set behavior for scripting.
	* \ingroup gp
	**/
	template<class T, class value_parser = CValueParser<T> >
	class _NRExport CDatorList : public IDator{
	protected:
		
	
		// Value/Target to be stored in
		std::list<T>& values;
		
		// parser
		SharedPtr<value_parser> valParser;
	
			
		/**
		* Convert given string to value of type T
		**/
		virtual T toVal(const std::string &s){
			
			try{
				return boost::lexical_cast<T>(s);
			}catch (boost::bad_lexical_cast &expr){
				if (valParser != NULL){
					return valParser->parse(s);
				}
				// We do not know what to return, so throw an error
				throw (expr);
			}
		}
		
		
		/**
		* Convert given value to a string
		**/
		virtual std::string toString(const T &val){
			
			try{
				return boost::lexical_cast<std::string>(val);
			}catch (boost::bad_lexical_cast &expr){
				if (valParser != NULL){
					return valParser->convert(val);
				}
				// We do not know what to return, so throw an error
				throw (expr);
			}
		}	
		
	public:
		
		/**
		* Create a dator list from a list. You can also specify value parser which will be used to parse
		* values that can not be casted lexicaly by ::boost
		* @param t Variable
		* @param parser Smart pointer to a value parser
		**/
		CDatorList(std::list<T>& t, SharedPtr<value_parser> parser = SharedPtr<value_parser>()):values(t), valParser(parser){}
		
		/**
		* Copy constructor
		**/
		CDatorList(const IDator &s) {
			// is not so pretty, but works. TODO: rewrite it
			if (s.hasMultipleValues()){
				valParser = (static_cast<CDatorList<T, value_parser> >(s)).valParser;
			}else{
				valParser = (static_cast<CDator<T, value_parser> >(s)).valParser;
			}
			
			values = static_cast<CDatorList<T, value_parser> >(s);
			//values.clear();
			//values.push_back(toVal(std::string(s)));
		}
		
		// access data methods
		T get(){return values.back();}
		operator std::list<T>&() { return values; }
		
		// access data methods
		operator std::string() 		{ return toString(values.back()); }
		const char* str() 			{ return toString(values.back()).c_str();}
		virtual operator int() 		{ return static_cast<int>(values.back()); }
		virtual operator bool() 	{ return static_cast<bool>(values.back()); }
		virtual operator float() 	{ return static_cast<float>(values.back()); }
				
		// operators to handle with strings
		IDator &operator =(const std::string &s)	 	{values.clear(); values.push_back(toVal(s)); return *this; }
		IDator &operator +=(const std::string &s)	 	{ values.push_back(toVal(s)); return *this; }
		IDator &operator -=(const std::string &s) 		{ values.remove(toVal(s)); return *this; }
		bool operator ==(const std::string &s)	 		{ return (find(values.begin(),values.end(),toVal(s))!=values.end());}
		bool operator !=(const std::string &s) 			{  return !((*this)==s); }
			
		// operators to other IDator objects
		IDator &operator =(const T &s)	{(*this)=toString(s); return *this;}
		IDator &operator +=(const T &s)	{(*this)+=toString(s); return *this;}
		IDator &operator -=(const T &s)	{(*this)-=toString(s); return *this;}
		bool operator ==(const T &s)		{return ((*this) == toString(s));}
		bool operator !=(const T &s)		{return ((*this) != toString(s));}
		
		// other methods
		bool hasMultipleValues() 			{ return true; }
	
	};

}; // end namespace

#endif	//_NRCDATOR_H_
