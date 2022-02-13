

//-----------------------------------------------------------------------------------------------

	// cout << setprecision(10);

//-----------------------------------------------------------------------------------------------


void copyStringNewFile(ifstream& inData, ofstream& outData)
{
    outData << inData.rdbuf();
}

//-----------------------------------------------------------------------------------------------


#include <cstdio>
 std::string name1 = std::tmpnam(nullptr);
 std::FILE* tmpf = std::tmpfile();

//-----------------------------------------------------------------------------------------------


inline void redistribute_vertices ( const Mesh & msh,    // line 710
  const Cell & start, const Cell & stop, double last_length, size_t n )
// hidden in anonymous namespace    // called only once

// perhaps just make some baricenters ?

// chain of n segments, desired length d, last length d'
// move p1 with   (d'-d) / n
//    alpha x0 + beta x2 == x1 + (d'-d) / n   alpha+beta == 1
//    x2 == x0 + 2d   x1 == x0 + d
//    2 d beta == d + (d'-d)/ n 
// move p2 with 2*(d'-d) / n
//    alpha x1 + beta x3 == x2 + 2(d'-d)/n   alpha+beta == 1
//    x1 == x0 + d + (d'-d)/n
//    x3 == x0 + 3d
//    alpha (d'-d)/n + 2 d beta == d + 2(d'-d)/n
//    ( 2d - (d'-d)/n ) beta == d + (d'-d)/n
// move p2 with 3*(d'-d) / n
//    ( 2d - 2(d'-d)/n ) beta == d + (d'-d)/n

// how to do this if the metric is not uniform ?
// use sums of lengths os segments, mimiking geodesics
	
{	Cell A = msh .cell_behind (stop) .base() .reverse();
	for ( size_t i = 1; i < n; i++ )
	{	if ( A == start )  {  n = i;  break;  }
		A = msh .cell_behind (A) .base() .reverse();  }
	assert ( n > 1 );
	Cell B = msh .cell_in_front_of (A) .tip();
	Cell C = msh .cell_in_front_of (B) .tip();
	double epsilon = ( last_length - desired_len_at_point ) / n;
	double v1 = desired_len_at_point + epsilon,  v2 = 2*desired_len_at_point;
	while ( C != stop )
	{	double beta = v1/v2;
		Manifold::working .interpolate ( B, 1.-beta, A, beta, C );
		v2 -= epsilon;  assert ( v2 > 0. );
		A = B;  B = C;
		C = msh .cell_in_front_of (B) .tip();                          }
	Manifold::working.interpolate ( B, 0.5, A, 0.5, C );                         }

//-----------------------------------------------------------------------------------------------


// flow of 'set_of_nearby_ver'in global.cpp :

void progress_relocate
// we compute here 'set_of_ver'
// (which is the set of all vertices in the cloud close enough to 'ver')
// and keep it for future use in 'check_touching'

inline bool check_touching
// we take advantage of 'set_of_ver' which is the set of all vertices in the cloud
// close enough to 'ver', previously computed in 'progress_relocate'
// we can destroy it here, it won't be used anymore
	if ( not ver.exists() )  return false;  // no touch

void progressive_construct ( args )

	Cell vertex_recently_built ( tag::non_existent );

restart:
angles_60 :

		// triangle waiting to be filled; see paragraph 12.7 in the manual
			if ( ver_next_to_B == A )  // this is the last triangle in this piece of progress_interface
			{	... goto search_for_start;    }
			... goto angles_60;               }

	bool touch = check_touching < manif_type >
		( vertex_recently_built, set_of_nearby_vertices,
      point_120, stop_point_120, cloud              );
	vertex_recently_built = Cell ( tag::non_existent );
	if ( touch )
	{	... goto angles_60;   }
  ...
// look for angles around 120 deg :
  ...
		if ( progress_cos_sq_120 ( A, point_120, B, prev_seg, next_seg) < 0.55 )  // 0.67
		// angle around 120 deg, we want to form two triangles; see paragraph 12.7 in the manual
		{	if ( ver_prev_to_A == ver_next_to_B )  // quadrangle
			{ ...		goto search_for_start;  }
			Cell P ( tag::vertex );  vertex_recently_built = P;
      ...
			progress_relocate < manif_type > ( P, 2, sum_of_nor, set_of_nearby_vertices, cloud );
      ...
			goto angles_60;                                                        }
  ...
// build a brand new triangle :
  ...
	Cell P ( tag::vertex );  vertex_recently_built = P;
  ....
	progress_relocate < manif_type > ( P, 1, f, set_of_nearby_vertices, cloud );
  ...
	goto angles_60;
...
search_for_start :  // execution only reaches this point through 'goto'

//-----------------------------------------------------------------------------------------------


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
	
//-----------------------------------------------------------------------------------------------


inline void print_spin ( Function::CompositionOfActions a )
{	std::cout << "{";
	auto it = a.index_map.begin();
	for ( ; it != a.index_map.end(); it++ )
		std::cout << it->first.id << ":" << it->second << ",";
	std::cout << "}" << std::endl;                           }



//-----------------------------------------------------------------------------------------------

bool correctly_oriented_complicated ( const Mesh msh )

// tells whether 'msh's orientation is consistent with the orientation of the
// surrounding Euclidian space

{	Manifold::Implicit::OneEquation * m_impl =
		dynamic_cast<Manifold::Implicit::OneEquation*> ( Manifold::working.core );
	assert ( m_impl );
	Manifold::Euclid * m_euclid =
		dynamic_cast<Manifold::Euclid*> ( m_impl->surrounding_space.core );
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

//-----------------------------------------------------------------------------------------------

