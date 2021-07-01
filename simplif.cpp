
// manifem/simplif.cpp 2021.03.21

//    This file is part of maniFEM, a C++ library for meshes and finite elements on manifolds.

//    Copyright 2019, 2020, 2021 Cristian Barbarosie cristian.barbarosie@gmail.com
//    https://github.com/cristian-barbarosie/manifem

//    ManiFEM is free software: you can redistribute it and/or modify it
//    under the terms of the GNU Lesser General Public License as published
//    by the Free Software Foundation, either version 3 of the License
//    or (at your option) any later version.

//    ManiFEM is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//    See the GNU Lesser General Public License for more details.

//    You should have received a copy of the GNU Lesser General Public License
//    along with maniFEM.  If not, see <https://www.gnu.org/licenses/>.

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <list>
#include <vector>
#include <map>
#include <set>
#include "assert.h"

namespace tag
{	struct Copy { };  static const Copy copy;
	struct Constant { };  static const Constant constant;
	struct AsConstAs { };  static const AsConstAs as_const_as;
	struct Integer { };  static const Integer integer;
	struct FromString { };  static const FromString from_string;
	struct Symbol { };  static const Symbol symbol;     }


struct Code

{	virtual ~Code ( ) { }
	
	enum From { from_void, sum, product, power };
	// enum From { from_void, first_in_sum, sum, first_in_product, product, power };

	virtual double price ( Code::From from = Code::from_void ) = 0;

	virtual std::string repr ( Code::From from = Code::from_void ) const = 0;

	struct Price { double sum, prod, power;  };

	static Price price_of;
};


Code::Price Code::price_of { 1., 2., 3. };

//  10^10 somas :     61s   
//  10^10 produtos :  62s
//  10^10 divisoes : 395s
//  10^9 potencias : 377s

//  10^10 soma + soma :     92s   
//  10^10 soma + prod :     91s   
//  10^10 soma + div  :    425s   

// uma subtracao tem o mesmo custo com uma soma

//	double a, b, c, z;
//	std::cin >> a >> b >> c;
//	for ( size_t i = 0; i < 100000; i++ )
//	for ( size_t j = 0; j < 100000; j++ )
//		z = a/b+c;

// custos : { sum: 3, prod: 3, div: 36, power: 374 }
	

struct ExprCore : Code

{	bool constant;

	std::string name;

	inline ExprCore ( ) : constant { false }  { }

	inline ExprCore ( const std::string s ) : constant { false }, name { s }  { }

	inline ExprCore ( const tag::Copy &, const ExprCore * const source )
	:	constant { source->constant }, name { source->name }  { }

	inline ExprCore ( const tag::Constant &, const std::string s )
	:	constant { true }, name { s }  { }
		
	virtual bool equal_to
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  ) = 0;
	virtual bool equal_to_int
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  ) = 0;
	virtual bool equal_to_sum
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  ) = 0;
	virtual bool equal_to_prod
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  ) = 0;
	virtual bool equal_to_pow
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  ) = 0;
	virtual bool equal_to_sym
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  ) = 0;
	
	virtual std::shared_ptr < ExprCore > sum_with
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  ) = 0;
	virtual std::shared_ptr < ExprCore > sum_with_int
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  ) = 0;
	virtual std::shared_ptr < ExprCore > sum_with_sum
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  ) = 0;
	virtual std::shared_ptr < ExprCore > sum_with_prod
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  ) = 0;
	virtual std::shared_ptr < ExprCore > sum_with_pow
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  ) = 0;
	virtual std::shared_ptr < ExprCore > sum_with_sym
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  ) = 0;
	
	virtual std::shared_ptr < ExprCore > prod_with
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  ) = 0;
	virtual std::shared_ptr < ExprCore > prod_with_int
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  ) = 0;
	virtual std::shared_ptr < ExprCore > prod_with_sum
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  ) = 0;
	virtual std::shared_ptr < ExprCore > prod_with_prod
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  ) = 0;
	virtual std::shared_ptr < ExprCore > prod_with_pow
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  ) = 0;
	virtual std::shared_ptr < ExprCore > prod_with_sym
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  ) = 0;
	
	virtual std::shared_ptr < ExprCore > power
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  ) = 0;
	virtual std::shared_ptr < ExprCore > power_of_int
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  ) = 0;
	virtual std::shared_ptr < ExprCore > power_of_sum
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  ) = 0;
	virtual std::shared_ptr < ExprCore > power_of_sym
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  ) = 0;

	// double price ( Code::From from = Code::from_void )   stays pure virtual from Code

	// std::string repr ( Code::From from = Code::from_void ) const
	//    stays pure virtual from Code

	inline std::string repr_or_name ( Code::From from = Code::from_void )
	{	if ( name.size() == 0 ) return repr ( from );
		return name;                                   }
			
};


struct Integer : ExprCore

{	int val;
	
	inline Integer ( int i ) : ExprCore ( tag::constant, "" ), val { i } { }

	inline Integer ( const tag::Copy &, const Integer * const source )
	: ExprCore ( tag::copy, source ), val { source->val } { }
	
	bool equal_to  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_int  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_sum  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_prod  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_pow  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_sym  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );

	std::shared_ptr < ExprCore > sum_with  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_int  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_sum  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_prod  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_pow  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_sym  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );

	std::shared_ptr < ExprCore > prod_with  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_int  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_sum  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_prod  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_pow  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_sym  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );

	std::shared_ptr < ExprCore > power  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > power_of_int  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > power_of_sum  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > power_of_sym  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	
	inline Integer * copy ( )
	{	return new Integer ( tag::copy, this );  }

	double price ( Code::From from = Code::from_void );  // virtual from Code, through ExprCore

	std::string repr ( Code::From from = Code::from_void ) const
	// virtual from Code, through ExprCore
	{	std::stringstream ss;
		ss << val; 
		std::string s = ss.str() + '.';
		if ( val < 0 ) s = "(" + s + ")";
		return s;                                   }
};


struct Sum : ExprCore
{
	std::vector < std::shared_ptr < ExprCore > > terms;

	inline Sum ( )  { }
	
	inline Sum ( const std::string s ) : ExprCore ( s )  { }

	inline Sum ( const tag::Copy &, const Sum * const source )
	: ExprCore ( tag::copy, source ), terms { source->terms }  { }
	
	bool equal_to  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_int  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_sum  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_prod  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_pow  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_sym  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );

	std::shared_ptr < ExprCore > sum_with  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_int  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_sum  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_prod  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_pow  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_sym  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );

	std::shared_ptr < ExprCore > prod_with  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_int  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_sum  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_prod  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_pow  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_sym  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );

	std::shared_ptr < ExprCore > power  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > power_of_int  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > power_of_sum  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > power_of_sym  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	
	inline Sum * copy ()
	{	return new Sum ( tag::copy, this );  }

	double price ( Code::From from = Code::from_void );  // virtual from Code, through ExprCore

	std::string repr ( Code::From from = Code::from_void ) const
	// virtual from Code, through ExprCore
	{	std::string s;
		std::vector<std::shared_ptr<ExprCore>>::const_iterator it;
		for ( it = terms.begin(); it != terms.end(); it++ )
			s += (*it)->repr_or_name() + "+";
		s = std::string ( s, 0, s.size()-1 );
		while ( true )
		{	size_t pos = s.find ("+(-1.)*");
			if ( pos == std::string::npos ) break;
			s = s.replace ( pos, 7, "-" );           }
		while ( true )
		{	size_t pos = s.find ("+(-1.)/");
			if ( pos == std::string::npos ) break;
			s = s.replace ( pos, 7, "-1./" );           }
		if ( ( from == Code::product ) or ( from == Code::power ) )
			s = "(" + s + ")";
		// if ( constant ) s = "C" + s;
		return s;                                                             }
};


struct Product : ExprCore

{	std::vector < std::shared_ptr < ExprCore > > factors;

	inline Product ( )  { }
	
	inline Product ( const std::string s ) : ExprCore ( s ) { }

	inline Product ( const tag::Copy &, const Product * const source )
	:	ExprCore ( tag::copy, source ), factors { source->factors } { }
	
	bool equal_to  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_int  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_sum  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_prod  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_pow  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_sym  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );

	std::shared_ptr < ExprCore > sum_with  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_int  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_sum  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_prod  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_pow  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_sym  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );

	std::shared_ptr < ExprCore > prod_with  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_int  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_sum  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_prod  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_pow  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_sym  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );

	std::shared_ptr < ExprCore > power  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > power_of_int  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > power_of_sum  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > power_of_sym  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	
	inline Product * copy ()
	{	return new Product ( tag::copy, this );  }

	double price ( Code::From from = Code::from_void );  // virtual from Code, through ExprCore

	std::string repr ( Code::From from = Code::from_void ) const
	// virtual from Code, through ExprCore
	{	std::string s;
		std::vector<std::shared_ptr<ExprCore>>::const_reverse_iterator it;
		for ( it = factors.rbegin(); it != factors.rend(); it++ )
			s += (*it)->repr_or_name ( Code::product ) + "*";
		s = std::string ( s, 0, s.size()-1 );
		size_t pos = s.find ("*(-1)");
		if ( pos == s.size()-5 ) s = "(-1)*" + s.substr ( 0, s.size()-5 );
		while ( true )
		{	size_t pos = s.find ("*1/");
			if ( pos == std::string::npos ) break;
			s = s.replace ( pos, 3, "/" );           }
		if ( from == Code::power ) s = "(" + s + ")";
		// if ( constant ) s = "C" + s;
		return s;                                                             }
};


struct Power : ExprCore
{
	std::shared_ptr < ExprCore > base;
	std::shared_ptr < ExprCore > exponent;

	inline Power ( std::shared_ptr < ExprCore > b, std::shared_ptr < ExprCore > e  )
	: base { b }, exponent { e }
	{	constant = base->constant and exponent->constant;  }

	inline Power ( const std::string s, std::shared_ptr < ExprCore > b,
                                      std::shared_ptr < ExprCore > e  )
	:	ExprCore ( s ), base { b }, exponent { e }
	{	constant = base->constant and exponent->constant;  }

	inline Power ( const tag::Copy &, const Power * const source )
	:	ExprCore ( tag::copy, source ), base { source->base },
		exponent { source->exponent }                                  { }
	
	bool equal_to  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_int  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_sum  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_prod  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_pow  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_sym  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );

	std::shared_ptr < ExprCore > sum_with  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_int  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_sum  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_prod  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_pow  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_sym  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );

	std::shared_ptr < ExprCore > prod_with  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_int  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_sum  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_prod  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_pow  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_sym  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );

	std::shared_ptr < ExprCore > power  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > power_of_int  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > power_of_sum  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > power_of_sym  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	
	inline Power * copy ()
	{	return new Power ( tag::copy, this );  }

	double price ( Code::From from = Code::from_void );  // virtual from Code, through ExprCore

	std::string repr ( Code::From from = Code::from_void ) const
	// virtual from Code, through ExprCore
	{	std::string s;
		Integer * exp_int = dynamic_cast < Integer * > ( this->exponent.get() );
		if ( ( exp_int ) and ( exp_int->val == -1 ) )
			s = "1./" + base->repr_or_name( Code::power );
		else s = "pow(" + base->repr_or_name() + "," + exponent->repr_or_name() + ")";
		// if ( constant ) s = "C" + s;
		return s;                                                          }
		
};


struct Symbol : ExprCore

{	inline Symbol ( std::string s )
	:	ExprCore { s } { }
	
	bool equal_to  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_int  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_sum  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_prod  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_pow  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	bool equal_to_sym  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );

	std::shared_ptr < ExprCore > sum_with  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_int  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_sum  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_prod  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_pow  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > sum_with_sym  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );

	std::shared_ptr < ExprCore > prod_with  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_int  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_sum  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_prod  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_pow  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > prod_with_sym  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );

	std::shared_ptr < ExprCore > power  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > power_of_int  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > power_of_sum  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > power_of_pow  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	std::shared_ptr < ExprCore > power_of_sym  // virtual from ExprCore
	( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  );
	
	double price ( Code::From from = Code::from_void );  // virtual from Code, through ExprCore

	std::string repr ( Code::From from = Code::from_void ) const
	// virtual from Code, through ExprCore
	{	return name;  }
};


struct Expression  // a wrapper for ExpressionCore

{
	std::shared_ptr < ExprCore > core;

	inline explicit Expression ( ) { }
	
	inline explicit Expression ( std::shared_ptr < ExprCore > c )
	:	core { c }  { }
	
	inline explicit Expression ( const tag::Integer &, int i )
	:	core ( new Integer { i } ) { }
		
	inline explicit Expression ( const tag::Symbol &, std::string s )
	:	core ( new Symbol { s } ) { }
};


struct Callable  // a whole C++ function

{	std::list < std::shared_ptr < ExprCore > > instr;
	// expressions, aka lines of code
	
	std::map < std::string, std::shared_ptr < ExprCore > > target_by_name;
	// return values

	double price;

	inline Callable ( const tag::Copy &, const Callable & source )
	:	instr { source.instr }, target_by_name { source.target_by_name }  { }
	
	inline Callable ( const tag::FromString &, std::string s );

	inline void get_instr ( const tag::FromString &, std::string s );

	static std::string name;  // function name
	static std::map < std::string, std::shared_ptr < Symbol > > argument_by_name;
	// input arguments
	
	std::string repr ( bool instr_only = false ) const;

	double compute_price ( );
};

std::string Callable::name;  // function name

std::map < std::string, std::shared_ptr < Symbol > > Callable::argument_by_name;
// input arguments


std::string Callable::repr ( bool instr_only ) const
// 'instr_only' defaults to 'false', so may be called with no arguments
	
{	std::string s;
	if ( not instr_only )
	{	s += "void " + Callable::name + " (";
		std::map<std::string,std::shared_ptr<Symbol>>::const_iterator it_arg;
		for ( it_arg = Callable::argument_by_name.begin();
					it_arg != Callable::argument_by_name.end(); it_arg++ )
		{	s += " double " + it_arg->first + ",";
			assert ( it_arg->first == it_arg->second->name );  }
		std::map<std::string,std::shared_ptr<ExprCore>>::const_iterator it_targ;
		for ( it_targ = this->target_by_name.begin();
					it_targ != this->target_by_name.end(); it_targ++ )
		{	s += " double & " + it_targ->first + ",";
			assert ( it_targ->first == it_targ->second->name );  }
		s = std::string ( s, 0, s.size()-1 ) + " )\n{\n";                           }
	std::list<std::shared_ptr<ExprCore>>::const_iterator it_instr;
	for ( it_instr = this->instr.begin(); it_instr != this->instr.end(); it_instr++ )
	{	ExprCore * instr = it_instr->get();
		s += "  ";
		if ( instr->constant ) s += "static const ";
		std::map<std::string,std::shared_ptr<ExprCore>>::const_iterator
			it_targ = this->target_by_name.find(instr->name);
		if ( it_targ == this->target_by_name.end() )
			s += "double ";
		else  assert ( not instr->constant );
		s += instr->name + " = ";
		s += instr->repr() + ";\n";            }
	if ( not instr_only )
	{	std::map<std::string,std::shared_ptr<ExprCore>>::const_iterator it_targ;
		for ( it_targ = this->target_by_name.begin();
		      it_targ != this->target_by_name.end(); it_targ++ )
			s += "  // " + it_targ->first + " = " + it_targ->second->repr() + ";\n";
		s += "}\n";                                                                 }
	return s;                                                                           }	


inline bool operator== ( const Expression & a, const Expression & b  )
{	return a.core->equal_to ( a.core, b.core );  }
// yes, we provide two pointers to the same object, a.core (shared) as first argument
// and a.core.get() (naked pointer) as 'this'


bool Integer::equal_to  // a.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	return b->equal_to_int ( a, b );  }  // yes, we provide 'b' twice


bool Sum::equal_to  // a.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	return b->equal_to_sum ( a, b );  }  // yes, we provide 'b' twice


bool Product::equal_to  // a.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	return b->equal_to_prod ( a, b );  }  // yes, we provide 'b' twice


bool Power::equal_to  // a.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	return b->equal_to_pow ( a, b );  }  // yes, we provide 'b' twice


bool Symbol::equal_to  // a.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	return b->equal_to_sym ( a, b );  }  // yes, we provide 'b' twice


bool Integer::equal_to_int  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// both 'a' and 'b' are Integer,  b.get() == this
{
#ifndef NDEBUG  // debug mode
	Integer * a_int = dynamic_cast < Integer * > ( a.get() );
	assert ( a_int );
#else  // optimized
	Integer * a_int = static_cast < Integer * > ( a.get() );
#endif
	return a_int->val == this->val;  }


bool Sum::equal_to_int  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Integer,  b.get() == this

{	return false;  }


bool Product::equal_to_int  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Integer,  b.get() == this

{	return false;  }


bool Power::equal_to_int  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Integer,  b.get() == this

{	return false;  }


bool Symbol::equal_to_int  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Integer,  b.get() == this

{	return false;  }


bool Integer::equal_to_sum  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Sum,  b.get() == this

{	return false;  }


bool Sum::equal_to_sum  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// both 'a' and 'b' are Sum,  b.get() == this
{	
#ifndef NDEBUG  // debug mode
	Sum * a_sum = dynamic_cast < Sum * > ( a.get() );
	assert ( a_sum );
#else  // optimized
	Sum * a_sum = static_cast < Sum * > ( a.get() );
#endif
	if ( a_sum->terms.size() != this->terms.size() ) return false;
	Sum * b_copy = this->copy();
	std::vector<std::shared_ptr<ExprCore>>::const_iterator it_a;
	for ( it_a = a_sum->terms.begin() ; it_a != a_sum->terms.end(); it_a++ )
	{	bool found = false;
		std::vector<std::shared_ptr<ExprCore>>::iterator it_b;
		for ( it_b = b_copy->terms.begin() ; it_b != b_copy->terms.end(); it_b++ )
			if ( (*it_a)->equal_to ( *it_a, *it_b ) )
			{	*it_b = std::shared_ptr < ExprCore > ( new Integer {0} );
				found = true;  break;                                     }
		if ( not found ) return false;                                             }
	return true;                                                                    }	


bool Product::equal_to_sum  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Sum,  b.get() == this

{	return false;  }


bool Power::equal_to_sum  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Sum,  b.get() == this

{	return false;  }


bool Symbol::equal_to_sum  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Sum,  b.get() == this

{	return false;  }


bool Integer::equal_to_prod  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Product,  b.get() == this

{	return false;  }


bool Sum::equal_to_prod  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Product,  b.get() == this

{	return false;  }


bool Product::equal_to_prod  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// both 'a' and 'b' are Product,  b.get() == this
{	
#ifndef NDEBUG  // debug mode
	Product * a_prod = dynamic_cast < Product * > ( a.get() );
	assert ( a_prod );
#else  // optimized
	Product * a_prod = static_cast < Product * > ( a.get() );
#endif
	if ( a_prod->factors.size() != this->factors.size() ) return false;
	Product * b_copy = this->copy();
	std::vector<std::shared_ptr<ExprCore>>::const_iterator it_a;
	for ( it_a = a_prod->factors.begin() ; it_a != a_prod->factors.end(); it_a++ )
	{	bool found = false;
		std::vector<std::shared_ptr<ExprCore>>::iterator it_b;
		for ( it_b = b_copy->factors.begin() ; it_b != b_copy->factors.end(); it_b++ )
			if ( (*it_a)->equal_to ( *it_a, *it_b ) )
			{	*it_b = std::shared_ptr < ExprCore > ( new Integer {1} );
				found = true;  break;                                     }
		if ( not found ) return false;                                                 }
	return true;                                                                        }	


bool Power::equal_to_prod  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Product,  b.get() == this

{	return false;  }


bool Symbol::equal_to_prod  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Product,  b.get() == this

{	return false;  }


bool Integer::equal_to_pow  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Power,  b.get() == this

{	return false;  }


bool Sum::equal_to_pow  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Power,  b.get() == this

{	return false;  }


bool Product::equal_to_pow  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Power,  b.get() == this

{	return false;  }


bool Power::equal_to_pow  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// both 'a' and 'b' are Power,  b.get() == this
{	
#ifndef NDEBUG  // debug mode
	Power * a_pow = dynamic_cast < Power * > ( a.get() );
	assert ( a_pow );
#else  // optimized
	Power * a_pow = static_cast < Power * > ( a.get() );
#endif
	std::shared_ptr < ExprCore > a_base = a_pow->base;
	if ( not a_base->equal_to ( a_base, this->base ) ) return false;
	std::shared_ptr < ExprCore > a_exp = a_pow->exponent;
	if ( not a_exp->equal_to ( a_exp, this->exponent ) ) return false;
	return true;                                                         }	


bool Symbol::equal_to_pow  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Power,  b.get() == this

{	return false;  }


bool Integer::equal_to_sym  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Symbol,  b.get() == this

{	return false;  }


bool Sum::equal_to_sym  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Symbol,  b.get() == this

{	return false;  }


bool Product::equal_to_sym  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Symbol,  b.get() == this

{	return false;  }


bool Power::equal_to_sym  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Symbol,  b.get() == this

{	return false;  }


bool Symbol::equal_to_sym  // virtual from ExprCore
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// both 'a' and 'b' are Power,  b.get() == this

{	return a.get() == this;  }  // a Symbol is only equal to itself


inline Expression operator+ ( const Expression & a, const Expression & b  )

{	return Expression { a.core->sum_with ( a.core, b.core ) };  }
// yes, we provide two pointers to the same object, a.core (shared) as first argument
// and a.core.get() (naked pointer) as 'this'


std::shared_ptr < ExprCore > Integer::sum_with  // a.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	if ( this->val == 0 ) return b;
	return b->sum_with_int ( a, b );  }  // yes, we provide 'b' twice


std::shared_ptr < ExprCore > Sum::sum_with  // a.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	return b->sum_with_sum ( a, b );  }  // yes, we provide 'b' twice


std::shared_ptr < ExprCore > Product::sum_with  // a.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	return b->sum_with_prod ( a, b );  }  // yes, we provide 'b' twice


std::shared_ptr < ExprCore > Power::sum_with  // a.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	return b->sum_with_pow ( a, b );  }  // yes, we provide 'b' twice


std::shared_ptr < ExprCore > Symbol::sum_with  // a.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	return b->sum_with_sym ( a, b );  }  // yes, we provide 'b' twice


std::shared_ptr < ExprCore > Integer::sum_with_int
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// both 'a' and 'b' are Integer, a is not zero,  b.get() == this

{	if ( this->val == 0 ) return a;
	#ifndef NDEBUG  // debug mode
		Integer * a_int = dynamic_cast < Integer * > ( a.get() );
		assert ( a_int );
	#else  // optimized
		Integer * a_int = ( Integer * ) a.get();
	#endif
	return std::shared_ptr<ExprCore> ( new Integer { a_int->val + this->val } );  }


std::shared_ptr < ExprCore > Sum::sum_with_int
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is non-zero Integer,  b.get() == this

{	bool int_considered = false;
#ifndef NDEBUG  // debug mode
	Integer * a_int = dynamic_cast < Integer * > ( a.get() );
	assert ( a_int );
#else  // optimized
	Integer * a_int = ( Integer * ) a.get();
#endif
	std::shared_ptr < Sum > result = std::shared_ptr<Sum> ( new Sum );
	result->constant = this->constant;
	std::vector<std::shared_ptr<ExprCore>>::const_iterator it;
	for ( it = this->terms.begin(); it != this->terms.end(); it++ )
	{	std::shared_ptr < ExprCore > term_p = *it;
		ExprCore * term = term_p.get();
		Integer * term_int = dynamic_cast < Integer * > ( term );
		if ( term_int )
		{	assert ( not int_considered );
			int_considered = true;
			double val = a_int->val + term_int->val;
			if ( val != 0 ) result->terms.push_back
				( std::shared_ptr < ExprCore > ( new Integer ( val ) ) );      }
		else result->terms.push_back ( *it );                              }
	if ( not int_considered ) result->terms.push_back ( a );
	return result;                                                         }


std::shared_ptr < ExprCore > Product::sum_with_int
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is non-zero Integer,  b.get() == this

{	std::shared_ptr < Sum > result = std::shared_ptr<Sum> ( new Sum );
	result->constant = this->constant;
	result->terms.push_back ( a );
	result->terms.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Power::sum_with_int
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is non-zero Integer,  b.get() == this

{	std::shared_ptr < Sum > result = std::shared_ptr<Sum> ( new Sum );
	result->constant = this->constant;
	result->terms.push_back ( a );
	result->terms.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Symbol::sum_with_int
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is non-zero Integer,  b.get() == this

{	std::shared_ptr < Sum > result = std::shared_ptr<Sum> ( new Sum );
	result->terms.push_back ( a );
	result->terms.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Integer::sum_with_sum
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Sum,  b.get() == this

{	if ( this->val == 0 ) return a;
	return a->sum_with_int ( b, a );  }
// yes, we provide two pointers to the same object, 'a' (shared) as first argument
// and a.get() (naked pointer) as 'this'


std::shared_ptr < ExprCore > Sum::sum_with_sum
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Sum,  b.get() == this
{	
	#ifndef NDEBUG  // debug mode
		Sum * a_sum = dynamic_cast < Sum * > ( a.get() );
		assert ( a_sum );
	#else  // optimized
	  Sum * a_sum = static_cast < Sum * > ( a.get() );
	#endif
	std::shared_ptr < Sum > result = std::shared_ptr<Sum> ( a_sum->copy() );	
	std::vector<std::shared_ptr<ExprCore>>::const_iterator it;
	// add terms of 'b'
	for ( it = this->terms.begin(); it != this->terms.end(); it++ )
	{	std::shared_ptr < ExprCore > term = *it;
		std::shared_ptr < ExprCore > res = term->sum_with_sum ( result, term );
#ifndef NDEBUG  // debug mode
		std::shared_ptr < Sum > res_sum = std::dynamic_pointer_cast < Sum > ( res );
		assert ( res_sum );
#else  // optimized
		std::shared_ptr < Sum > res_sum = std::static_pointer_cast < Sum > ( res );
#endif
		result = res_sum;                                                            }	
	return result;                                                                         }


std::shared_ptr < ExprCore > Product::sum_with_sum
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Sum,  b.get() == this
{	
	#ifndef NDEBUG  // debug mode
		Sum * a_sum = dynamic_cast < Sum * > ( a.get() );
		assert ( a_sum );
	#else  // optimized
	  Sum * a_sum = ( Sum * ) a.get();
	#endif
	std::shared_ptr < Sum > result = std::shared_ptr<Sum> ( a_sum->copy() );	
	result->terms.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Power::sum_with_sum
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Sum,  b.get() == this
{	
#ifndef NDEBUG  // debug mode
	Sum * a_sum = dynamic_cast < Sum * > ( a.get() );
	assert ( a_sum );
#else  // optimized
	Sum * a_sum = ( Sum * ) a.get();
#endif
	std::shared_ptr < Sum > result = std::shared_ptr<Sum> ( a_sum->copy() );	
	result->terms.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Symbol::sum_with_sum
// 'a' is Sum,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
{	
#ifndef NDEBUG  // debug mode
	Sum * a_sum = dynamic_cast < Sum * > ( a.get() );
	assert ( a_sum );
#else  // optimized
	Sum * a_sum = ( Sum * ) a.get();
#endif
	std::shared_ptr < Sum > result = std::shared_ptr<Sum> ( a_sum->copy() );	
	result->terms.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Integer::sum_with_prod
// 'a' is Product,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	if ( this->val == 0 ) return a;
	std::shared_ptr < Sum > result = std::shared_ptr<Sum> ( new Sum );
	result->terms.push_back ( a );
	result->terms.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Sum::sum_with_prod
// 'a' is Product,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	std::shared_ptr < Sum > result = std::shared_ptr<Sum> ( this->copy() );	
	result->terms.push_back ( a );
	return result;                                                      } 


std::shared_ptr < ExprCore > Product::sum_with_prod
// 'a' is Product,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	std::shared_ptr < Sum > result = std::shared_ptr<Sum> ( new Sum );
	result->terms.push_back ( a );
	result->terms.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Power::sum_with_prod
// 'a' is Product,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	std::shared_ptr < Sum > result = std::shared_ptr<Sum> ( new Sum );
	result->terms.push_back ( a );
	result->terms.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Symbol::sum_with_prod
// 'a' is Product,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	std::shared_ptr < Sum > result = std::shared_ptr<Sum> ( new Sum );
	result->terms.push_back ( a );
	result->terms.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Integer::sum_with_pow
// 'a' is Power,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	if ( this->val == 0 ) return a;
	std::shared_ptr < Sum > result = std::shared_ptr<Sum> ( new Sum );
	result->terms.push_back ( a );
	result->terms.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Sum::sum_with_pow
// 'a' is Power,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
	
{	std::shared_ptr < Sum > result = std::shared_ptr<Sum> ( this->copy() );	
	result->terms.push_back ( a );
	return result;                                                          } 


std::shared_ptr < ExprCore > Product::sum_with_pow
// 'a' is Power,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	std::shared_ptr < Sum > result = std::shared_ptr<Sum> ( new Sum );
	result->terms.push_back ( a );
	result->terms.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Power::sum_with_pow
// 'a' is Power,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{ std::shared_ptr < Sum > result = std::shared_ptr<Sum> ( new Sum );
	result->terms.push_back ( a );
	result->terms.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Symbol::sum_with_pow
// 'a' is Power,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	std::shared_ptr < Sum > result = std::shared_ptr<Sum> ( new Sum );
	result->terms.push_back ( a );
	result->terms.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Integer::sum_with_sym
// 'a' is Symbol,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	if ( this->val == 0 ) return a;
	std::shared_ptr < Sum > result = std::shared_ptr<Sum> ( new Sum );
	result->terms.push_back ( a );
	result->terms.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Sum::sum_with_sym
// 'a' is Product,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
	
{	std::shared_ptr < Sum > result = std::shared_ptr<Sum> ( this->copy() );	
	result->terms.push_back ( a );
	return result;                                                          } 


std::shared_ptr < ExprCore > Product::sum_with_sym
// 'a' is Symbol,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	std::shared_ptr < Sum > result = std::shared_ptr<Sum> ( new Sum );
	result->terms.push_back ( a );
	result->terms.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Power::sum_with_sym
// 'a' is Symbol,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{ std::shared_ptr < Sum > result = std::shared_ptr<Sum> ( new Sum );
	result->terms.push_back ( a );
	result->terms.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Symbol::sum_with_sym
// 'a' is Symbol,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	if ( a.get() == this ) return b->prod_with_int
		( std::shared_ptr<Integer> ( new Integer {2} ), b );
	std::shared_ptr < Sum > result = std::shared_ptr<Sum> ( new Sum );
	result->terms.push_back ( a );
	result->terms.push_back ( b );
	return result;                                                      } 


inline Expression operator* ( const Expression & a, const Expression & b  )

{	return Expression { a.core->prod_with ( a.core, b.core ) };  }
// yes, we provide two pointers to the same object, a.core (shared) as first argument
// and a.core.get() (naked pointer) as 'this'


std::shared_ptr < ExprCore > Integer::prod_with  // a.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	if ( this->val == 1 ) return b;
	if ( this->val == 0 ) return std::shared_ptr<Integer> ( new Integer {0} );
	return b->prod_with_int ( a, b );  }  // yes, we provide 'b' twice


std::shared_ptr < ExprCore > Sum::prod_with  // a.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	return b->prod_with_sum ( a, b );  }  // yes, we provide 'b' twice


std::shared_ptr < ExprCore > Product::prod_with  // a.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	return b->prod_with_prod ( a, b );  }  // yes, we provide 'b' twice


std::shared_ptr < ExprCore > Power::prod_with  // a.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	return b->prod_with_pow ( a, b );  }  // yes, we provide 'b' twice


std::shared_ptr < ExprCore > Symbol::prod_with  // a.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	return b->prod_with_sym ( a, b );  }  // yes, we provide 'b' twice


std::shared_ptr < ExprCore > Integer::prod_with_int
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// both 'a' and 'b' are Integer, a is not zero and not one,  b.get() == this

{	if ( this->val == 0 ) return std::shared_ptr<ExprCore> ( new Integer {0} );
	if ( this->val == 1 ) return a;
#ifndef NDEBUG  // debug mode
	Integer * a_int = dynamic_cast < Integer * > ( a.get() );
	assert ( a_int );
#else  // optimized
	Integer * a_int = static_cast < Integer * > ( a.get() );
#endif
	return std::shared_ptr<ExprCore> ( new Integer { a_int->val * this->val } );  }


std::shared_ptr < ExprCore > Sum::prod_with_int
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Integer (not zero, not one), b.get() == this

{	std::shared_ptr < Product > result = std::shared_ptr<Product> ( new Product );
	result->factors.push_back ( a );
	result->factors.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Product::prod_with_int
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Integer (not zero, not one), b.get() == this

{	bool int_considered = false;
#ifndef NDEBUG  // debug mode
	Integer * a_int = dynamic_cast < Integer * > ( a.get() );
	assert ( a_int );
#else  // optimized
	Integer * a_int = static_cast < Integer * > ( a.get() );
#endif
	std::shared_ptr < Product > result = std::shared_ptr<Product> ( new Product );
	std::vector<std::shared_ptr<ExprCore>>::const_iterator it;
	for ( it = this->factors.begin(); it != this->factors.end(); it++ )
	{	std::shared_ptr < ExprCore > fact_p = *it;
		ExprCore * fact = fact_p.get();
		Integer * fact_int = dynamic_cast < Integer * > ( fact );
		if ( fact_int )
		{	assert ( not int_considered );
			int_considered = true;
			double val = a_int->val * fact_int->val;
			if ( val != 1 ) result->factors.push_back
				( std::shared_ptr < ExprCore > ( new Integer ( val ) ) );  }
		else result->factors.push_back ( *it );                          }
	if ( not int_considered ) result->factors.push_back ( a );
	return result;                                                                     }


std::shared_ptr < ExprCore > Power::prod_with_int
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Integer (not zero, not one), b.get() == this

{	std::shared_ptr < Product > result = std::shared_ptr<Product> ( new Product );
	result->factors.push_back ( b );
	result->factors.push_back ( a );
	return result;                                                                   } 


std::shared_ptr < ExprCore > Symbol::prod_with_int
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Integer (not zero, not one), b.get() == this

{	std::shared_ptr < Product > result = std::shared_ptr<Product> ( new Product );
	result->factors.push_back ( a );
	result->factors.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Integer::prod_with_sum
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Sum,  b.get() == this

{	std::shared_ptr < Product > result = std::shared_ptr<Product> ( new Product );
	result->factors.push_back ( a );
	result->factors.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Sum::prod_with_sum
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Sum,  b.get() == this

{	std::shared_ptr < Product > result = std::shared_ptr<Product> ( new Product );
	result->factors.push_back ( a );
	result->factors.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Product::prod_with_sum
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Sum,  b.get() == this

{	std::shared_ptr < Product > result = std::shared_ptr<Product> ( this->copy() );	
	result->factors.push_back ( a );
	return result;                                                      } 


std::shared_ptr < ExprCore > Power::prod_with_sum
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Sum,  b.get() == this

{	std::shared_ptr < Product > result = std::shared_ptr<Product> ( new Product );
	result->factors.push_back ( b );
	result->factors.push_back ( a );
	return result;                                                      } 


std::shared_ptr < ExprCore > Symbol::prod_with_sum
// 'a' is Sum,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	std::shared_ptr < Product > result = std::shared_ptr<Product> ( new Product );
	result->factors.push_back ( a );
	result->factors.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Integer::prod_with_prod
// 'a' is Product,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	if ( this->val == 1 ) return a;
	if ( this->val == 0 ) return std::shared_ptr<Integer> ( new Integer {0} );
	return a->prod_with_int ( b, a );  }
// yes, we provide two pointers to the same object, a.core (shared) as second argument
// and a.core.get() (naked pointer) as 'this'


std::shared_ptr < ExprCore > Sum::prod_with_prod
// 'a' is Product,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
	
{	return a->prod_with_sum ( b, a );  }
// yes, we provide two pointers to the same object, a.core (shared) as second argument
// and a.core.get() (naked pointer) as 'this'


std::shared_ptr < ExprCore > Product::prod_with_prod
// 'a' is Product,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
{
#ifndef NDEBUG  // debug mode
	Product * a_prod = dynamic_cast < Product * > ( a.get() );
	assert ( a_prod );
#else  // optimized
	Product * a_prod = static_cast < Product * > ( a.get() );
#endif
	std::shared_ptr < Product > result = std::shared_ptr<Product> ( a_prod->copy() );	
	std::vector<std::shared_ptr<ExprCore>>::const_iterator it;
	// add factors of 'b'
	for ( it = this->factors.begin(); it != this->factors.end(); it++ )
	{	std::shared_ptr < ExprCore > fact = *it;
		std::shared_ptr < ExprCore > res = fact->prod_with_prod ( result, fact );
#ifndef NDEBUG  // debug mode
		std::shared_ptr < Product > res_prod = 
			std::dynamic_pointer_cast < Product > ( res );
		assert ( res_prod );
#else  // optimized
	  	std::shared_ptr < Product > res_prod = 
			std::static_pointer_cast < Product > ( res );
#endif
		result = res_prod;                                                           }	
	return result;                                                                    }


std::shared_ptr < ExprCore > Power::prod_with_prod
// 'a' is Product,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	return a->prod_with_pow ( b, a );  }
// yes, we provide two pointers to the same object, a.core (shared) as second argument
// and a.core.get() (naked pointer) as 'this'


std::shared_ptr < ExprCore > Symbol::prod_with_prod
// 'a' is Product,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	return a->prod_with_sym ( b, a );  }
// yes, we provide two pointers to the same object, a.core (shared) as second argument
// and a.core.get() (naked pointer) as 'this'


std::shared_ptr < ExprCore > Integer::prod_with_pow
// 'a' is Power,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	std::shared_ptr < Product > result = std::shared_ptr<Product> ( new Product );
	result->factors.push_back ( a );
	result->factors.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Sum::prod_with_pow
// 'a' is Power,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	std::shared_ptr < Product > result = std::shared_ptr<Product> ( new Product );
	result->factors.push_back ( a );
	result->factors.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Product::prod_with_pow
// 'a' is Power,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{ std::shared_ptr < Product > result = std::shared_ptr<Product> ( this->copy() );	
	result->factors.push_back ( a );
	return result;                                                      } 


std::shared_ptr < ExprCore > Power::prod_with_pow
// 'a' is Power,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
{
#ifndef NDEBUG  // debug mode
	Power * a_pow = dynamic_cast < Power * > ( a.get() );
	assert ( a_pow );
#else  // optimized
	Power * a_pow = static_cast < Power * > ( a.get() );
#endif
	if ( a_pow->base->equal_to ( this->base, a_pow->base ) )
	{	std::shared_ptr < Power > result = std::shared_ptr < Power > ( a_pow->copy() );
	  result->exponent = a_pow->exponent->sum_with ( this->exponent, a_pow->exponent );
		return result;                                                                    }
	std::shared_ptr < Product > result = std::shared_ptr<Product> ( new Product );
	result->factors.push_back ( a );
	result->factors.push_back ( b );
	return result;                                                                        } 


std::shared_ptr < ExprCore > Symbol::prod_with_pow
// 'a' is Power,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	std::shared_ptr < Product > result = std::shared_ptr<Product> ( new Product );
	result->factors.push_back ( a );
	result->factors.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Integer::prod_with_sym
// 'a' is Symbol,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	std::shared_ptr < Product > result = std::shared_ptr<Product> ( new Product );
	result->factors.push_back ( a );
	result->factors.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Sum::prod_with_sym
// 'a' is Symbol,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	std::shared_ptr < Product > result = std::shared_ptr<Product> ( new Product );
	result->factors.push_back ( a );
	result->factors.push_back ( b );
	return result;                                                      } 


std::shared_ptr < ExprCore > Product::prod_with_sym
// 'a' is Symbol,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
	
{ std::shared_ptr < Product > result = std::shared_ptr<Product> ( this->copy() );	
	result->factors.push_back ( a );
	return result;                                                      } 


std::shared_ptr < ExprCore > Power::prod_with_sym
// 'a' is Symbol,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{ std::shared_ptr < Product > result = std::shared_ptr<Product> ( new Product );
	result->factors.push_back ( b );
	result->factors.push_back ( a );
	return result;                                                      } 


std::shared_ptr < ExprCore > Symbol::prod_with_sym
// 'a' is Symbol,  b.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	// if ( (*a) == (*b) ) return b->pow_with_int
	// 	( std::shared_ptr<Integer> ( new Integer {2} ), b );
	std::shared_ptr < Product > result = std::shared_ptr<Product> ( new Product );
	result->factors.push_back ( a );
	result->factors.push_back ( b );
	return result;                                                      } 


inline Expression power ( const Expression & a, const Expression & b )

{ return Expression ( a.core->power ( a.core, b.core ) );  }
// yes, we provide two pointers to the same object, a.core (shared) as first argument
// and a.core.get() (naked pointer) as 'this'


inline Expression power ( const Expression & a, int b )
{ return power ( a, Expression ( tag::integer, b ) );  }


std::shared_ptr < ExprCore > Integer::power  // a.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	if ( this->val == 0 ) return std::shared_ptr < ExprCore > ( new Integer ( 0 ) );
	if ( this->val == 1 ) return std::shared_ptr < ExprCore > ( new Integer ( 1 ) );
	return b->power_of_int ( a, b );  }  // yes, we provide 'b' twice


std::shared_ptr < ExprCore > Sum::power  // a.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	return b->power_of_sum ( a, b );  }  // yes, we provide 'b' twice


std::shared_ptr < ExprCore > Product::power  // a.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	std::shared_ptr < ExprCore > result = std::shared_ptr<ExprCore>
		( new Integer ( 1 ) );
	std::vector<std::shared_ptr<ExprCore>>::const_iterator it;
	for ( it = this->factors.begin(); it != this->factors.end(); it++ )
	{	std::shared_ptr < ExprCore > x = *it;
		result = result->prod_with ( result, x->power ( x, b ) );  }
	return result;                                                       }
	

std::shared_ptr < ExprCore > Power::power  // a.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	std::shared_ptr < ExprCore > bb = this->base;
	std::shared_ptr < ExprCore > ee = this->exponent;
	return bb->power ( bb, ee->prod_with ( ee, b ) );       }


std::shared_ptr < ExprCore > Symbol::power  // a.get() == this
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )

{	return b->power_of_sym ( a, b );  }  // yes, we provide 'b' twice


std::shared_ptr < ExprCore > Integer::power_of_int
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// both 'a' and 'b' are Integer, a is not zero and not one,  b.get() == this

{	if ( this->val == 0 ) return std::shared_ptr < ExprCore > ( new Integer ( 1 ) );
	if ( this->val == 1 ) return a;
	return std::shared_ptr<ExprCore> ( new Power ( a, b ) );                         }


std::shared_ptr < ExprCore > Sum::power_of_int
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Integer (not zero, not one),  b.get() == this

{	return std::shared_ptr<ExprCore> ( new Power ( a, b ) );  }


std::shared_ptr < ExprCore > Product::power_of_int
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Integer (not zero, not one),  b.get() == this

{	return std::shared_ptr<ExprCore> ( new Power ( a, b ) );  }


std::shared_ptr < ExprCore > Power::power_of_int
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Integer (not zero, not one),  b.get() == this

{	assert ( false );  } 


std::shared_ptr < ExprCore > Symbol::power_of_int
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Integer (not zero, not one),  b.get() == this

{	return std::shared_ptr<ExprCore> ( new Power ( a, b ) );  }


std::shared_ptr < ExprCore > Integer::power_of_sum
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Sum,  b.get() == this

{	if ( this->val == 0 ) return std::shared_ptr < ExprCore > ( new Integer ( 1 ) );
	if ( this->val == 1 ) return a;
	return std::shared_ptr<ExprCore> ( new Power ( a, b ) );                         }


std::shared_ptr < ExprCore > Sum::power_of_sum
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Sum,  b.get() == this

{	return std::shared_ptr<ExprCore> ( new Power ( a, b ) );  }


std::shared_ptr < ExprCore > Product::power_of_sum
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Sum,  b.get() == this

{	return std::shared_ptr<ExprCore> ( new Power ( a, b ) );  }


std::shared_ptr < ExprCore > Power::power_of_sum
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Sum,  b.get() == this

{	assert ( false );  } 


std::shared_ptr < ExprCore > Symbol::power_of_sum
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Sum,  b.get() == this

{	return std::shared_ptr<ExprCore> ( new Power ( a, b ) );  }


std::shared_ptr < ExprCore > Integer::power_of_sym
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Symbol,  b.get() == this

{	if ( this->val == 0 ) return std::shared_ptr < ExprCore > ( new Integer ( 1 ) );
	if ( this->val == 1 ) return a;
	return std::shared_ptr<ExprCore> ( new Power ( a, b ) );                         }


std::shared_ptr < ExprCore > Sum::power_of_sym
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Symbol,  b.get() == this

{	return std::shared_ptr<ExprCore> ( new Power ( a, b ) );  }


std::shared_ptr < ExprCore > Product::power_of_sym
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Symbol,  b.get() == this

{	return std::shared_ptr<ExprCore> ( new Power ( a, b ) );  }


std::shared_ptr < ExprCore > Power::power_of_sym
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Symbol,  b.get() == this

{	assert ( false );  } 


std::shared_ptr < ExprCore > Symbol::power_of_sym
( std::shared_ptr < ExprCore > a, std::shared_ptr < ExprCore > b  )
// 'a' is Symbol,  b.get() == this

{	return std::shared_ptr<ExprCore> ( new Power ( a, b ) );  }


double Integer::price ( Code::From from )  // 'from' defaults to Code::from_void

{	return 0.;  }


double Sum::price ( Code::From from )  // 'from' defaults to Code::from_void

{	if ( this->constant ) return 0.;
	return ( this->terms.size() - 1 ) * Code::price_of.sum;  }


double Product::price ( Code::From from )  // 'from' defaults to Code::from_void

{	if ( this->constant ) return 0.;
	double p = ( this->factors.size() -1 ) * Code::price_of.prod;
	std::vector<std::shared_ptr<ExprCore>>::const_iterator it;
	for ( it = this->factors.begin(); it != this->factors.end(); it++ )
	{	std::shared_ptr < ExprCore > fact = (*it);
		Integer * fact_int = dynamic_cast < Integer * > ( fact.get() );
		if ( ( fact_int ) and ( fact_int->val == -1 ) ) p -= Code::price_of.prod;;
		// product by -1 does not count
		Power * fact_pow = dynamic_cast < Power * > ( fact.get() );
		if ( fact_pow )
		{	Integer * exp_int = dynamic_cast < Integer * > ( fact_pow->exponent.get() );
			if ( ( exp_int ) and ( exp_int->val == -1 ) ) p -= Code::price_of.power;      }  }
		// power with exponent -1 is acutally division
	return p;                                                                               }


double Power::price ( Code::From from )  // 'from' defaults to Code::from_void

{	if ( this->constant ) return 0.;
	return Code::price_of.power;      }


double Symbol::price ( Code::From from )  // 'from' defaults to Code::from_void

{	return 0.;  }


inline Expression compl_item ( std::string & s, Callable * clbl );
inline Expression compl_item_for_sum ( std::string & s, Callable * clbl );
inline Expression compl_item_for_mul ( std::string & s, Callable * clbl );
inline Expression compl_item_for_pow ( std::string & s, Callable * clbl );
inline Expression compl_integer ( std::string & s, Callable * clbl );
// all modify s


inline Expression string_to_expression ( std::string s, Callable * clbl )
// modifies s

{ size_t i = 0;  while ( isspace(s[i]) ) i++;
	s = s.substr(i);
	assert ( s.size() > 0 );
	char c = s[0];
	assert ( c != '+' );
	assert ( c != '*' );
	assert ( c != '/' );
	assert ( c != '^' );
	Expression result;
	if ( c == '-' )
	{	s = std::string ( s, 1 );  // modifies s
		result = Expression ( tag::integer, -1 ) * compl_item_for_sum ( s, clbl );  }
	else result = compl_item_for_sum ( s, clbl );  // modifies s
	while ( true )
	{	size_t i = 0;  while ( isspace(s[i]) ) i++;
		s = s.substr(i);
		if ( s.size() == 0 ) return result;
		c = s[0];
		if ( c == '+' )
		{	s = std::string ( s, 1 );
			result = result + compl_item_for_sum ( s, clbl );
			// compl_item_for_sum modifies s
			continue;                                                          }
		if ( c == '-' )
		{	s = std::string ( s, 1 );
			result = result + Expression ( tag::integer, -1 ) * compl_item_for_sum ( s, clbl );
			// compl_item_for_sum modifies s
			continue;                                                                     }
		std::cout << "---- " << c << std::endl;
		assert ( false );
	}		
	assert ( false );
}


inline Expression compl_integer ( std::string & s, Callable * clbl )
// modifies s
	
{	assert ( s.size() > 0 );
	std::string item;
	while ( true )
	{	if ( s.size() == 0 ) break;
		char c = s[0];
		if ( ( '0' <= c ) and ( c <= '9' ) )
		{	item += c;
			s = std::string ( s, 1 );
			continue;                           }
		break;                                   }
	int i = stoi ( item );
	if ( s.size() )  if ( s[0] == '.' )  s = std::string ( s, 1 );
	return Expression ( tag::integer, i );                          }


inline Expression compl_item ( std::string & s, Callable * clbl )
// modifies s

{	assert ( s.size() > 0 );
	if ( s[0] == '(' )
	{	size_t counter = 1, i = 1;
		for ( ; i < s.size(); i++ )
		{	char c = s[i];
			if ( c == '(' ) counter++;
			if ( c == ')' ) counter--;
			if ( counter == 0 ) break;   }
		assert ( counter == 0 );
		std::string item = std::string ( s, 1, i-1 );
		s = std::string ( s, i+1 );
		return string_to_expression ( item, clbl );       }
	assert ( not ( ( '0' <= s[0] ) and ( s[0] <= '9' ) ) );
	size_t i = 0;
	while ( true )
	{	i++;
		if ( i >= s.size() ) break;
		char c = s[i];
		if ( ( 'a' <= c ) and ( c <= 'z' ) ) continue;
		if ( ( 'A' <= c ) and ( c <= 'Z' ) ) continue;
		if ( ( '0' <= c ) and ( c <= '9' ) ) continue;
		if ( c == '_' ) continue;
		break;                                            }
	std::string item ( s, 0, i );
	assert ( item.size() > 0 );
	s = std::string ( s, i );
	std::shared_ptr<ExprCore> result;
	std::map<std::string,std::shared_ptr<Symbol>>::iterator
		it_arg = Callable::argument_by_name.find(item);
	if ( it_arg != Callable::argument_by_name.end() )
		result = it_arg->second;
	else
	{	std::map<std::string,std::shared_ptr<ExprCore>>::iterator
			it_targ = clbl->target_by_name.find(item);
		if ( it_targ != clbl->target_by_name.end() )
			result = it_targ->second;
		else
		{	std::list<std::shared_ptr<ExprCore>>::iterator
				it_instr = clbl->instr.begin();
			bool found = false;
			for ( ; it_instr != clbl->instr.end(); it_instr++ )
			{	std::shared_ptr < ExprCore > instr = *it_instr;
				if ( instr->name == item )
				{ found = true;  result = instr;  break;  }        }
			assert ( found );                                       }  }
	return Expression ( result );                                    }	


inline Expression compl_item_for_sum ( std::string & s, Callable * clbl )
// modifies s

{	size_t i = 0;  while ( isspace(s[i]) ) i++;
	s = s.substr(i);
	assert ( s.size() > 0 );
  Expression item;
	char c = s[0];
	if ( ( '0' <= c ) and ( c <= '9' ) )
		item = compl_integer ( s, clbl );  // modifies 's'
	else
	{	assert ( ( ( 'a' <= c ) and ( c <= 'z' ) ) or
		         ( ( 'A' <= c ) and ( c <= 'Z' ) ) or
		         ( c == '(' )                         );
		item = compl_item ( s, clbl );                          }
	while ( true )
	{	if ( s.size() == 0 ) return item;
		c = s[0];
		if ( c == '*' )
		{	s = std::string ( s, 1 );
			item = item * compl_item_for_mul ( s, clbl ); // modifies 's'
			continue;                                                      }
		if ( c == '/' )
		{	s = std::string ( s, 1 );
			item = item * power ( compl_item_for_mul ( s, clbl ), -1 );
			continue;                                                      }
		if ( c == '^' )
		{	s = std::string ( s, 1 );
			c = s[0];
			if ( ( '0' <= c ) and ( c <= '9' ) )
				item = power ( item, compl_integer ( s, clbl ) );
			else item = power ( item, compl_item ( s, clbl ) );
			continue;                                                  }
		return item;                                                        }	 }


inline Expression compl_item_for_mul ( std::string & s, Callable * clbl )
// modifies s

{	assert ( s.size() > 0 );
  Expression item;
	char c = s[0];
	if ( ( '0' <= c ) and ( c <= '9' ) )
		item = compl_integer ( s, clbl );  // modifies 's'
	else if ( ( ( 'a' <= c ) and ( c <= 'z' ) ) or
		        ( ( 'A' <= c ) and ( c <= 'Z' ) ) or
	          ( c == '(' )                      )
		item = compl_item ( s, clbl );  // modifies 's'
	while ( true )
	{	if ( s.size() == 0 ) return item;
		c = s[0];
		if ( c == '^' )
		{	s = std::string ( s, 1 );
			c = s[0];
			if ( ( '0' <= c ) and ( c <= '9' ) )
				item = power ( item, compl_integer ( s, clbl ) );  // modifies 's'
			else item = power ( item, compl_item ( s, clbl ) );
			continue;                                                  }
		return item;                                                    }  }


inline std::string compl_name ( std::string & s )
// modifies s
	
{	size_t i = 0;  while ( isspace(s[i]) ) i++;
	s = s.substr(i);
	char c = s[0];
	assert ( ( ( 'a' <= c ) and ( c <= 'z' ) ) or
	         ( ( 'A' <= c ) and ( c <= 'Z' ) )     );
	i = 0;
	while ( true )
	{	i++;
		if ( i >= s.size() ) break;
		char c = s[i];
		if ( ( 'a' <= c ) and ( c <= 'z' ) ) continue;
		if ( ( 'A' <= c ) and ( c <= 'Z' ) ) continue;
		if ( ( '0' <= c ) and ( c <= '9' ) ) continue;
		if ( c == '_' ) continue;
		break;                                            }
	std::string item ( s, 0, i );
	assert ( item.size() > 0 );
	s = s.substr ( i );
	return item;                                            }


inline Callable::Callable ( const tag::FromString &, std::string s )
// destroys s

{	size_t i = 0;  while ( isspace(s[i]) ) i++;
	s = s.substr ( i );
	assert ( s.substr ( 0, 4 ) == "void" );
	i = 4;
	while ( isspace(s[i]) ) i++;
	s = s.substr ( i );
	Callable::name = compl_name ( s );  // modifies s
	i = s.find ( '(' );
	while ( true )  // input arguments
	{	i++;  while ( isspace(s[i]) ) i++;
		s = s.substr (i);
		assert ( s.substr ( 0, 6 ) == "double" );
		i = 6;  while ( isspace(s[i]) ) i++;
		if ( s[i] == '&' ) break;
		assert ( s[i] != ')' );
		s = s.substr (i);
		std::string arg = compl_name ( s );  // modifies s
		Callable::argument_by_name [ arg ] = std::shared_ptr < Symbol > ( new Symbol ( arg ) );
		i = 0;  while ( isspace(s[i]) ) i++;
		assert ( s[i] == ',' );                                                                  }
	while ( true )  // targets (output)
	{	i++;  while ( isspace(s[i]) ) i++;
		s = s.substr (i);
		std::string targ = compl_name ( s );  // modifies s
		assert ( this->target_by_name.find(targ) == this->target_by_name.end() );
		Callable::target_by_name [ targ ] = std::shared_ptr < Symbol > ( new Symbol ( targ ) );
		i = 0;  while ( isspace(s[i]) ) i++;
		if ( s[i] == ')' ) break;
		assert ( s[i] == ',' );
		i++;  while ( isspace(s[i]) ) i++;
		s = s.substr (i);
		assert ( s.substr ( 0, 6 ) == "double" );
		i = 6;  while ( isspace(s[i]) ) i++;                           }		
	i++;  while ( isspace(s[i]) ) i++;
	assert ( s[i] == '{' );
	i++;  while ( isspace(s[i]) ) i++;
	s = s.substr (i);
	i = s.find('}');
	this->get_instr ( tag::from_string, s.substr ( 0, i ) );
}


inline void Callable::get_instr ( const tag::FromString &, std::string s )

{	size_t i = 0;  while ( isspace(s[i]) ) i++;
	s = s.substr (i);
	std::set < std::string > target_names;
	std::map<std::string,std::shared_ptr<ExprCore>>::iterator it;
	for ( it = this->target_by_name.begin(); it != this->target_by_name.end(); it++ )
	{	assert ( target_names.find ( it->first ) == target_names.end() );
		target_names.insert ( it->first );                                 }
	assert ( target_names.size() == this->target_by_name.size() );
	while ( true )  // instructions (statements)
	{	std::string targ = compl_name ( s );  // modifies s
		i = 0;  while ( isspace(s[i]) ) i++;
		bool sta = targ == "static";
		if ( sta )
		{	targ = compl_name ( s );  // modifies s
			i = 0;  while ( isspace(s[i]) ) i++;
			assert ( targ == "const" );        
			targ = compl_name ( s );  // modifies s
			i = 0;  while ( isspace(s[i]) ) i++;
			assert ( targ == "double" );        
			targ = compl_name ( s );  // modifies s
			i = 0;  while ( isspace(s[i]) ) i++;  }
		else if ( targ == "double" )
		{	targ = compl_name ( s );  // modifies s
			i = 0;  while ( isspace(s[i]) ) i++;  }
		else assert ( target_names.find(targ) != target_names.end() );
		assert ( s[i] == '=' ); 
		i++;  while ( isspace(s[i]) ) i++;
		s = s.substr (i);
		i = s.find ( ';' );
		std::shared_ptr < ExprCore > tt = string_to_expression ( s.substr ( 0, i ), this ) .core;
		tt->name = targ;
		if ( target_names.find(targ) != target_names.end() )
		{	target_names.erase(targ);
			this->target_by_name [ targ ] = tt;  }
		this->instr.push_back ( tt );
		i++;  while ( isspace(s[i]) ) i++;
		if ( i == s.size() ) break;
		s = s.substr (i);                                                              }
	assert ( target_names.size() == 0 );
}


int main ( )
{
	std::ifstream f_in ( "simplif.dat" );
	std::string line, s;
	while ( true )
	{	getline ( f_in, line );
		if ( line.size() == 0 ) break;
		s += line;                 }
	Callable original ( tag::from_string, s );
	std::cout << original.repr();
	std::list < Callable > population;
	line = "";  size_t i;
	while ( true )
	{	while ( f_in )
		{	getline ( f_in, line );
		  i = 0;  while ( isspace(line[i]) ) i++;
			line = line.substr (i);
			if ( line.size() ) break;      }
		if ( not f_in ) break;
		assert ( line.substr ( 0, 5 ) == "price" );
	  i = 5;  while ( isspace(line[i]) ) i++;
		line = line.substr (i);
		double price = stod ( line );
		std::cout << std::endl << "price " << price << std::endl;
		s = "";
		while ( f_in )
		{	getline ( f_in, line );
			i = 0;  while ( isspace(line[i]) ) i++;
			line = line.substr (i);
			if ( line.size() == 0 ) break;
			assert ( line[line.size()-1] == ';' );
			s += line;                               }
		population.emplace_back ( tag::copy, original );
		Callable & func = population.back();
		func.price = price;
		func.instr.clear();
		func.get_instr ( tag::from_string, s );
		std::cout << func.repr ( true );                      }
}
