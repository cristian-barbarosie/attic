
inline Function Function::multivalued
( const tag::Through &, const Function::Action & g,
  const tag::Becomes &, const Function & f         )

{	return Function ( tag::whose_core_is,
		new Function::Scalar::MultiValued ( *this, tag::through, g, tag::becomes, f ) );  }


inline Function Function::multivalued
( const tag::Through &, const Function::Action & g1,
  const tag::Becomes &, const Function & f1,
  const tag::Through &, const Function::Action & g2,
  const tag::Becomes &, const Function & f2         )

{	return Function ( tag::whose_core_is,
		new Function::Scalar::MultiValued ( *this, tag::through, g1, tag::becomes, f1,
		                                           tag::through, g2, tag::becomes, f2 ) );  }


inline Function Function::multivalued
( const tag::Through &, const Function::Action & g1,
  const tag::Becomes &, const Function & f1,
  const tag::Through &, const Function::Action & g2,
  const tag::Becomes &, const Function & f2,
  const tag::Through &, const Function::Action & g3,
  const tag::Becomes &, const Function & f3         )

{	return Function ( tag::whose_core_is,
		new Function::Scalar::MultiValued ( *this, tag::through, g1, tag::becomes, f1,
		                                           tag::through, g2, tag::becomes, f2,
		                                           tag::through, g3, tag::becomes, f3 ) );  }


//-------------------------------------------------------------------------------------------------

	Cell::Numbering::Map numbering;
	{ // just a block of code for hiding 'it' and 'counter'
	CellIterator it = ABCD.iterator ( tag::over_vertices );
	size_t counter = 0;
	for ( it.reset() ; it.in_range(); it++ )
	{	Cell V = *it;  numbering ( V ) = counter;  ++counter;  }
	} // just a block of code



//-------------------------------------------------------------------------------------------------

	Cell::Numbering & numbering = * ( fe.numbers[0] );


	// unfortunately, the numbering provided by fe.numbers[0] is not contiguous
	// so we fill the main diagonal with ones
	// then we put zero for vertices belonging to ABCD
	for ( size_t i = 0; i < size_matrix; i++ )
		matrix_A.insert ( i, i ) = 1.;
	{ // just a block of code for hiding 'it'
	CellIterator it = ABCD.iterator ( tag::over_vertices );
	for ( it.reset(); it.in_range(); it++ )
	{	Cell P = *it;
		size_t i = numbering ( P );
		matrix_A.coeffRef ( i, i ) = 0.;  }
	} // just a block of code 




//-------------------------------------------------------------------------------------------------

bool correctly_oriented_complicated ( const Mesh msh )

// tells whether 'msh's orientation is consistent with the orientation of the
// surrounding Euclidian space

{	Manifold::Implicit::OneEquation * m_impl =
		dynamic_cast<Manifold::Implicit::OneEquation*> ( Manifold::working.core );
	assert ( m_impl );
	Manifold::Euclid * m_euclid = dynamic_cast<Manifold::Euclid*> ( m_impl->surrounding_space.core );
	assert ( m_euclid );

	if ( progress_nb_of_coords != 2 )
	{	std::cout << "for the moment I can only check the orientation of "
	            << "(closed) curves in the plane - sorry" << std::endl;
		exit ( 1 );                                                         }

	Function x = Manifold::working.coordinates()[0];
	Function y = Manifold::working.coordinates()[1];
	CellIterator it = msh.iterator ( tag::over_segments, tag::require_order );
	it.reset();  assert ( it.in_range() );
	Cell seg = *it;
	assert ( seg.dim() == 1 );
	bool dx_pos = ( x (seg.tip()) - x (seg.base().reverse()) ) > 0.;
	bool dy_pos = ( y (seg.tip()) - y (seg.base().reverse()) ) > 0.;
	int counter = 0;
	for ( it++; it.in_range(); it++ )
	{	seg = *it;
		bool dx_pos_now = ( x(seg.tip()) - x (seg.base().reverse()) ) > 0.;
		bool dy_pos_now = ( y(seg.tip()) - y (seg.base().reverse()) ) > 0.;
		if ( dx_pos_now != dx_pos )  //  dx has changed sign
		{	if ( dy_pos_now != dy_pos )  // dy has changed sign, too
			{	std::cout << "I cannot check the orientation if the curve "
			            << "has too sharp angles - sorry" << std::endl;
				// we can do better here
				exit ( 1 );                                                  }
			if ( dx_pos == dy_pos ) counter++;
			else counter--;                                                    }
		dx_pos = dx_pos_now;  dy_pos = dy_pos_now;                              }
	assert ( ( counter == 2 ) or ( counter == -2 ) );
	return counter == 2;                                                         }

//-------------------------------------------------------------------------------------------------

