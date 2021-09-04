

void print_segment ( Cell seg )

{	Manifold::Quotient * manif_q = dynamic_cast
		< Manifold::Quotient* > ( Manifold::working.core );
	assert ( manif_q );
	Function xy = manif_q->base_space.coordinates();
	Function x = xy[0], y = xy[1];
	size_t n = manif_q->actions.size();
	assert ( n == manif_q->spins.size() );
	Cell V = seg.base().reverse();
	Cell W = seg.tip();
	std::cout << "[(" << x(V) << "," << y(V) << "),(" << x(W) << "," <<y(W) << ")] ";
	Function::CompositionOfActions a = seg.spin();
	std::cout << "(";
	for ( size_t i = 0; i < n; i++ )
	{	Function::Action & g = manif_q->actions[i];
		std::map<Function::Action,short int>::const_iterator itt = a.index_map.find ( g );
		if ( itt == a.index_map.end() )
		{	std::cout << "0,"; continue;  }
		short int exp = itt->second;
		assert ( exp != 0 );
		std::cout << exp << ",";                                                            }
	std::cout << ")" << std::endl;                                                           }
	

//-------------------------------------------------------------------------------------------------

inline void print_spin ( Function::CompositionOfActions a )
{	std::cout << "{";
	auto it = a.index_map.begin();
	for ( ; it != a.index_map.end(); it++ )
		std::cout << it->first.id << ":" << it->second << ",";
	std::cout << "}" << std::endl;                           }



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

