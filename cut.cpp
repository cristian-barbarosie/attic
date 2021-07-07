// src/manifem/current/main-cut.cpp  2019.08.28

#include <fstream>
#include "Mesh.h"
#include <cmath>

using namespace std;

// we consider a mesh and a cut (an interface) which is a level set of
// a scalar function psi
// we then deform locally the mesh, adjusting it to the cut


void special_draw ( Mesh & square, Mesh & cut, std::string f )

{	assert ( Mesh::environment != NULL );
	NumericField &coord = * ( Mesh::environment->coord_field );

	assert (coord.size() == 2);
	// we split 'coord' into its components
	OneDimField &x = coord[0], &y = coord[1];

	double xmin, xmax, ymin, ymax, maxside;
	
	{ // just a block for hiding variables
		CellIterator it = square.iter_over ( tag::cells, tag::of_dim, 0, tag::not_oriented );
	it.reset(); assert ( it.in_range() );
	Cell & Vfirst = *it;
	xmin = xmax = x(Vfirst);
	ymin = ymax = y(Vfirst);	
	for ( it++; it.in_range(); it++ )
	{ Cell & V = *it; 
		double xV = x(V), yV = y(V);
		if ( xV < xmin ) xmin = xV;
	  if ( xV > xmax ) xmax = xV;
	  if ( yV < ymin ) ymin = yV;
	  if ( yV > ymax ) ymax = yV;      }
	} // just a block for hiding variables 
	if ( xmax-xmin < ymax-ymin ) maxside = ymax-ymin;
	else maxside = xmax-xmin; 
	double border = 0.02*maxside;
	double scale_factor = 500/maxside;
	double translation_x = -scale_factor*xmin;
	double translation_y = -scale_factor*ymin;
	
	std::ofstream file_ps (f);
	file_ps << "%!PS-Adobe-3.0 EPSF-3.0" << std::endl;
	file_ps << "%%Title:                     malha" << std::endl;
	file_ps << "%%BoundingBox:  " << -border*scale_factor << " " << -border*scale_factor << " "
					<< scale_factor*(xmax+border) + translation_x << "   "
					<< scale_factor*(ymax+border) + translation_y << std::endl;
	file_ps << "%%EndComments" << std::endl << std::endl;

	file_ps << "gsave" << std::endl;
	// file_ps << "/m{moveto}def" << std::endl;
	// file_ps << "/l{lineto}def" << std::endl;
	// file_ps << "/s{stroke}def" << std::endl;
	file_ps << translation_x << " " << translation_y << " translate" << std::endl;
	file_ps << scale_factor << " " << scale_factor << " scale" << std::endl << std::endl;
	
	file_ps << "gsave 0.004 setlinewidth" << std::endl;
	std::list<Cell*>::iterator it = square.cells[1]->begin(),
		e = square.cells[1]->end();
	for ( ; it != e; it++)
	{
		Cell & seg = *(*it);
		Cell & base = seg.base().reverse();
		Cell & tip  = seg.tip();
		file_ps << x(base) << " " << y(base) << " moveto" << std::endl;
		file_ps << x(tip) << " " << y(tip) << " lineto stroke" << std::endl;
	}
	
#ifndef NDEBUG
	file_ps << "/Courier findfont 0.2 scalefont setfont" << std::endl;
	it = square.cells[0]->begin();
	e = square.cells[0]->end();
	for ( ; it != e; it++)
	{	Cell & p = *(*it);
		if ( p.name() == "" ) continue;
		file_ps << x(p) << " " << y(p) << " moveto (" << p.name() << ") show" << std::endl;  }
#endif

	file_ps << "0.8 0 0 setrgbcolor 0.007 setlinewidth" << std::endl;
	CellIterator itt = cut.iter_over ( tag::segments, tag::along );
	for ( itt.reset(); itt.in_range(); itt++ )
	{	Cell & seg = *itt;
		Cell & base = seg.base().reverse();
		Cell & tip  = seg.tip();
		file_ps << x(base) << " " << y(base) << " moveto" << std::endl;
		file_ps << x(tip) << " " << y(tip) << " lineto stroke" << std::endl;  }		
	
	file_ps << "grestore" << std::endl;
	file_ps << "grestore" << std::endl;
	file_ps << "showpage" << std::endl;
	file_ps << "%%Trailer" << std::endl;
	file_ps << "%EOF" << std::endl;

	if ( ! file_ps.good() )
	{	std::cerr << "error writing postscript file" << std::endl;
		exit (1);                                    }

} // end of special_draw


bool opposite_signs ( double a, double b )
{	return ( ( a >= 0. ) and ( b < 0. ) ) or ( ( a < 0. ) and ( b >= 0. ) );  }


void analyse_angles_left ( Mesh & square, Mesh & interface, Cell & seg )

{	assert ( Mesh::environment != NULL );
	NumericField &coord = * ( Mesh::environment->coord_field );

	assert (coord.size() == 2);
	// we split 'coord' into its components
	OneDimField &x = coord[0], &y = coord[1];

	bool first_pass = true;
	Cell * seg_p = &seg;
	while ( true )
	{	if ( seg_p == NULL ) return;
		Cell & seg1 = * seg_p;
		Cell & P = seg1.base().reverse();
		Cell & Q = seg1.tip();
		double dx = x(Q) - x(P), dy = y(Q) - y(P),
			norm = std::sqrt(dx*dx+dy*dy), cos_angle;
		Cell * cll_ptr = square.cell_behind ( seg1, tag::may_not_exist );
		if ( cll_ptr == NULL ) return;
		Cell & cll= *cll_ptr;
		if ( not cll.belongs_to(square) ) return;
		// if ( cll.meshes[2]->find(&square) == cll.meshes[2]->end() ) return;
		Cell & seg2 = cll.boundary().cell_behind(P);
		Cell & R = seg2.base().reverse();
		double dx2 = x(R) - x(P), dy2 = y(R) - y(P),
			norm2 = std::sqrt(dx2*dx2+dy2*dy2);
		if ( cll.boundary().cells[1]->size() == 3 ) goto next_neighbour_cell;
		assert ( cll.boundary().cells[1]->size() == 4 );
		cos_angle = ( dx*dx2 + dy*dy2 ) / norm / norm2;
		if ( cos_angle < -0.3 ) // wide angle
			cll.split ( tag::in_two_triangles, tag::at, P, tag::cell_is_rectangle );
		else if ( cos_angle > 0.3 ) // sharp angle
			cll.split ( tag::in_two_triangles, tag::at, Q, tag::cell_is_rectangle );
	next_neighbour_cell :
		first_pass = false;
		if ( seg2.belongs_to(interface) ) return;
		// if ( pos_seg2->meshes[1]->find(&interface) != pos_seg2->meshes[1]->end() )
		// 	return;
		seg_p = seg2.hidden_reverse;                                              }

} // end of analyse_angles_left


void analyse_angles_right ( Mesh & square, Mesh & interface, Cell & seg )

{	assert ( Mesh::environment != NULL );
	NumericField &coord = * ( Mesh::environment->coord_field );

	assert (coord.size() == 2);
	// we split 'coord' into its components
	OneDimField &x = coord[0], &y = coord[1];

	bool first_pass = true;
	Cell * seg_p = &seg;
	while ( true )
	{	if ( seg_p == NULL ) return;
		Cell & seg1 = * seg_p;
		Cell & P = seg1.base().reverse();
		Cell & Q = seg1.tip();
		double dx = x(Q) - x(P), dy = y(Q) - y(P),
			norm = std::sqrt(dx*dx+dy*dy), cos_angle;
		Cell * cll_ptr = square.cell_in_front_of ( seg1, tag::may_not_exist );
		if ( cll_ptr == NULL ) return;
		Cell & cll= *cll_ptr;
		if ( not cll.belongs_to(square) ) return;
		// if ( cll.meshes[2]->find(&square) == cll.meshes[2]->end() ) return;
		Cell & seg2 = cll.boundary().cell_in_front_of(P);
		Cell & R = seg2.tip();
		double dx2 = x(R) - x(P), dy2 = y(R) - y(P),
			norm2 = std::sqrt(dx2*dx2+dy2*dy2);
		if ( cll.boundary().cells[1]->size() == 3 ) goto next_neighbour_cell;
		assert ( cll.boundary().cells[1]->size() == 4 );
		cos_angle = ( dx*dx2 + dy*dy2 ) / norm / norm2;
		if ( cos_angle < -0.3 ) // wide angle
			cll.split ( tag::in_two_triangles, tag::at, P, tag::cell_is_rectangle );
		else if ( cos_angle > 0.3 ) // sharp angle
			cll.split ( tag::in_two_triangles, tag::at, Q, tag::cell_is_rectangle );
	next_neighbour_cell :
		first_pass = false;
		if ( seg2.belongs_to(interface) ) return;
		// if ( pos_seg2->meshes[1]->find(&interface) != pos_seg2->meshes[1]->end() )
		// 	return;
		seg_p = &seg2;                                                              }
} // end of analyse_angles_right


Mesh & build_interface ( Mesh & ambient, Mesh & bdry, set<Cell*> & corners,
                         FunctionOnMesh::Function & psi                     )

// 'ambient' is a mesh of rectangles, or at least in the neighbourhood of
// the set  psi == 0  it is composed of rectangles only
	
// within the mesh 'ambient', look for vertices close to the level set  psi == 0
// slightly change the position of these vertices to satisfy  psi == 0
// in some cases, split a rectangle in two triangles

// the search starts on 'bdry' which is the boundary of 'ambient'
// and stops on 'bdry', too
// the algorithm will only detect one connected component of the level set

{	FunctionOnMesh::Function & xy = * ( Mesh::environment->coord_func );
	FunctionOnMesh::Function & x = xy[0], & y = xy[1];

	FunctionOnMesh::Function psi_x = psi.deriv(x), psi_y = psi.deriv(y);

	// to each vertex we associate a map from neighbour vertices to cells
	// this third cell will be a segment if the two vertices belong to the same segment
	// will be a square if the two vertices are diagonally opposed in that square
	map < Cell*, map <Cell*,Cell*> > neighbours;

	// we begin by searching a segment in 'bdry' where psi changes sign
	// and such that 'seg' and grad psi are in sharp angle
	// that is, psi increases along 'seg'
	Cell * keep = NULL;
	{ // just a block for making 'it' local
	CellIterator it = bdry.iter_over ( tag::segments, tag::around );
	for ( it.reset(); it.in_range(); it++ )
	{	Cell & seg = *it;
		Cell & AA = seg.base().reverse();
		Cell & BB = seg.tip();
		double xa = x(AA), ya = y(AA), xb = x(BB), yb = y(BB),
			delta_x = xb-xa, delta_y = yb-ya;
		double psi1 = psi (AA);
		double psi2 = psi (BB);
		if ( ( opposite_signs ( psi1, psi2 ) ) and
				 ( psi_x(AA)*delta_x + psi_y(AA)*delta_y >= 0. ) )
			keep = & seg;                                         }
	} // just a block for making 'it' local
	if ( keep == NULL )
	{	cerr << "cannot start process of building the interface" << endl;
		cout << "cannot start process of building the interface" << endl;
		exit (0);                                                          }
	Cell & first_seg = * keep;

	// below we build a chain of rectangles approximating the cut (interface)
	while ( keep != NULL )
	{	Cell * current_sq = ambient.cell_behind ( * keep, tag::may_not_exist );
		if ( current_sq == NULL ) break;
		assert ( current_sq->boundary().number_of ( tag::cells, tag::of_max_dim ) == 4 );
		CellIterator it1 = current_sq->boundary().iter_over ( tag::vertices, tag::around );
		for ( it1.reset(); it1.in_range(); it1++ )
		{	Cell & P = *it1;
			if ( neighbours.find(&P) == neighbours.end() )
				neighbours[&P] = map <Cell*,Cell*> ();
			CellIterator it2 = current_sq->boundary().iter_over ( tag::vertices, tag::around );
			it2.reset(P); assert ( it2.in_range() );
			it2++;  assert ( it2.in_range() );
			Cell & Q = *it2;
			Cell & PQ = current_sq->boundary().cell_in_front_of(P);
			assert ( & ( PQ.tip() ) == & Q );
			double dx = x(Q) - x(P);
			double dy = y(Q) - y(P);
			double gx = psi_x(P);
			double gy = psi_y(P);
			if ( gx*dy > gy*dx ) neighbours[&P][&Q] = &PQ;
			it2++;  assert ( it2.in_range() );
			Cell & R = *it2;
			dx = x(R) - x(P);
			dy = y(R) - y(P);
			if ( gx*dy > gy*dx ) neighbours[&P][&R] = current_sq;
			it2++;  assert ( it2.in_range() );
			Cell & S = *it2;
			Cell & PS = current_sq->boundary().cell_in_front_of(S).reverse();
			assert ( & ( PS.tip() ) == & S );
			assert ( & ( PS.base() ) == & ( P.reverse() ) );
			dx = x(S) - x(P);
			dy = y(S) - y(P);
			if ( gx*dy > gy*dx ) neighbours[&P][&S] = &PS;
			it2++;  assert ( not it2.in_range() );                                                 }
		CellIterator it = current_sq->boundary().iter_over ( tag::segments, tag::around );
		it.reset( *keep ); assert ( it.in_range() );
		bool found = false;
		for ( it++; it.in_range(); it++ )
		{	Cell & seg = *it;
			double psi1 = psi ( seg.tip() );
			double psi2 = psi ( seg.base().reverse() );
			if ( opposite_signs ( psi1, psi2 ) )
			{	found = true; keep = seg.hidden_reverse; break;  }                     }
		assert ( found );                                                                      }
	// in each square, psi changes sign

	// begin building the interface
	Mesh & interface = * ( new Mesh (1) );
	// looking for the first point :
	{ // a block for hiding variables AA and BB
	Cell & AA = first_seg.base().reverse();
	Cell & BB = first_seg.tip();
	if ( abs ( psi(AA) ) < abs ( psi(BB) ) ) keep = &AA;
	else keep = &BB;
	} // a block for hiding variables AA and BB
	while ( true )
	{	Cell * previous_point = keep;
		map <Cell*,Cell*> & nei = neighbours[keep];
		map <Cell*,Cell*>::iterator it = nei.begin();
		if ( it == nei.end() ) break;
		keep = it->first;
		double min_psi = abs ( psi(*keep) );
		for ( it++; it != nei.end(); it++ )
		{	Cell * Q = it->first;
			double val_psi = abs ( psi(*Q) );
			if ( val_psi < min_psi )
			{	min_psi = val_psi;
				keep = Q;           }                             }
		Cell * cll = nei[keep];
		if ( cll->dim == 1 ) cll->add_to ( interface );
		else
		{	assert ( cll->dim == 2 );
			Cell & new_seg = cll->split ( tag::in_two_triangles, tag::at, *previous_point,
			                              tag::cell_is_rectangle                           );
			assert ( & ( new_seg.base() ) == & ( previous_point->reverse() ) );
			keep = & ( new_seg.tip() );
			new_seg.add_to ( interface );                                                         }
		if ( keep->belongs_to(bdry) ) break;
		// if ( keep->meshes[1]->find(&bdry) != keep->meshes[1]->end()) break;
	} // end of 'while true'

	// changing the interface's shape to match the level set
	// apply Newton's method and baricenter
	for ( short int i = 0; i < 5; i++ )
	{	CellIterator it = interface.iter_over ( tag::segments, tag::along );
		it.reset(); assert ( it.in_range() );
		Cell & first_point = (*it).base().reverse();
		Cell * last = NULL;
		for ( it++; it.in_range(); it++ )
		{	Cell & seg = *it;
			Cell & P = seg.base().reverse();
			last = & ( seg.tip() );
			// Newton's method
			double Px = x(P), Py = y(P);
			double val_psi = psi(P);
			double gx = psi_x(P);
			double gy = psi_y(P);
			double norm__grad_sq = gx*gx + gy*gy;
			Px -= val_psi * gx / norm__grad_sq;
			Py -= val_psi * gy / norm__grad_sq;
			FunctionOnMesh::prescribe_on (P); x == Px; y == Py;
		}  // end of for it
		Cell & last_point	= * last;
		{ // inner block for hiding variables
		Cell & P = first_point;
		// Newton's method
		// if ( ( &P != &A ) and ( &P != &B ) and ( &P != &C ) and ( &P != &D ) )
		if ( corners.find(&P) == corners.end() )
		{	double Px = x(P), Py = y(P);
			double val_psi = psi(P);
			double gx = psi_x(P);
			double gy = psi_y(P);
			Cell & seg = bdry.cell_in_front_of (P);
			Cell & Q = seg.tip();
			double direc_x = x(Q) - x(P), direc_y = y(Q) - y(P);
			double direc_grad = direc_x*gx + direc_y*gy;
			Px -= val_psi / direc_grad * direc_x ;
			Py -= val_psi / direc_grad * direc_y ;
			FunctionOnMesh::prescribe_on (P); x == Px; y == Py;   }
		} { // inner block for hiding variables
		Cell & P = last_point;
		// Newton's method
		// if ( ( &P != &A ) and ( &P != &B ) and ( &P != &C ) and ( &P != &D ) )
		if ( corners.find(&P) == corners.end() )
		{	double Px = x(P), Py = y(P);
			double val_psi = psi(P);
			double gx = psi_x(P);
			double gy = psi_y(P);
			Cell & seg = bdry.cell_in_front_of (P);
			Cell & Q = seg.tip();
			double direc_x = x(Q) - x(P), direc_y = y(Q) - y(P);
			double direc_grad = direc_x*gx + direc_y*gy;
			Px -= val_psi / direc_grad * direc_x ;
			Py -= val_psi / direc_grad * direc_y ;
			FunctionOnMesh::prescribe_on (P); x == Px; y == Py;   }
		} // inner block for hiding variables
		// baricenter along the interface
		it.reset(); assert ( it.in_range() );
		double previous_x = x(first_point), previous_y = y(first_point);
		for ( it++; it.in_range(); it++ )
		{	Cell & seg = *it;
			Cell & P = seg.base().reverse();
			double Px = x(P), Py = y(P);
			double val_psi = psi(P);
			double gx = psi_x(P);
			double gy = psi_y(P);
			double norm__grad_sq = gx*gx + gy*gy;
			Cell & Q = seg.tip();
			double dx = ( previous_x + x(Q) ) / 2. - Px,
			       dy = ( previous_y + y(Q) ) / 2. - Py;
			double d_grad = dx * gx + dy * gy;
			dx -= d_grad / norm__grad_sq * gx;
			dy -= d_grad / norm__grad_sq * gy;
			Px += dx;  Py += dy;
			FunctionOnMesh::prescribe_on (P); x == Px; y == Py;
			previous_x = Px;
			previous_y = Py;
		}  // end of for it
	} // end of 'for i'

	// apply Newton's method only
	for ( short int i = 0; i < 2; i++ )
	{	CellIterator it = interface.iter_over ( tag::segments, tag::along );
		it.reset(); assert ( it.in_range() );
		for ( it++; it.in_range(); it++ )
		{	Cell & seg = *it;
			Cell & P = seg.base().reverse();
			// Newton's method
			double Px = x(P), Py = y(P);
			double val_psi = psi(P);
			double gx = psi_x(P);
			double gy = psi_y(P);
			double norm__grad_sq = gx*gx + gy*gy;
			Px -= val_psi * gx / norm__grad_sq;
			Py -= val_psi * gy / norm__grad_sq;
			FunctionOnMesh::prescribe_on (P); x == Px; y == Py;  }                  }
	
	// apply baricenter for the first layer of neighbours
	set < Cell * > layer;
	{	// outer block of code for hiding variables
	CellIterator it = interface.iter_over ( tag::segments, tag::along );
	it.reset(); assert ( it.in_range() );
	{ // intermediate block of code for hiding variables
	Cell & seg = *it;
	Cell & P = seg.base().reverse();
	assert ( P.belongs_to(bdry) );
	assert ( P.belongs_to(interface) );
	assert ( P.belongs_to(ambient) );
	// assert ( P.meshes[1]->find(&bdry) != P.meshes[1]->end() );
	// assert ( P.meshes[1]->find(&interface) != P.meshes[1]->end() );
	// assert ( P.meshes[2]->find(&ambient) != P.meshes[2]->end() );
	{ // inner block of code for hiding variables
	Cell & sseg = bdry.cell_in_front_of (P);
	Cell & PP = sseg.tip();
	assert ( not PP.belongs_to(interface) );
	// assert ( PP.meshes[1]->find(&interface) == PP.meshes[1]->end() );
	layer.insert (&PP);
	} // inner block of code for hiding variables
	CellIterator itt = ambient.iter_over ( tag::cells, tag::above, P,
																				 tag::of_dim_plus_one, tag::along );
	for ( itt.reset(); itt.in_range(); itt++ )
	{	Cell & sseg = *itt;
		Cell & PP = sseg.base().reverse();
		if ( not PP.belongs_to(interface) )
			// if ( PP.meshes[1]->find(&interface) == PP.meshes[1]->end() )
			layer.insert (&PP);                                         }
	} // intermediate block of code for hiding variables
	Cell * last_seg;
	for ( it++; it.in_range(); it++ )
	{	Cell & seg = *it;
		last_seg = & seg;
		Cell & P = seg.base().reverse();
		CellIterator itt = ambient.iter_over ( tag::cells, tag::above, P,
																					 tag::of_dim_plus_one, tag::around );
		for ( itt.reset(); itt.in_range(); itt++ )
		{	Cell & sseg = *itt;
			Cell & PP = sseg.base().reverse();
			if ( not PP.belongs_to(interface) )
				// if ( PP.meshes[1]->find(&interface) == PP.meshes[1]->end() )
				layer.insert (&PP);                                              }   }
	{ // intermediate block of code for hiding variables
	Cell & P = last_seg->tip();
	{ // inner block of code for hiding variables
	Cell & sseg = bdry.cell_in_front_of (P);
	Cell & PP = sseg.tip();
	assert ( not PP.belongs_to(interface) );
	// assert ( PP.meshes[1]->find(&interface) == PP.meshes[1]->end() );
	layer.insert (&PP);
	} // inner block of code for hiding variables
	CellIterator itt = ambient.iter_over ( tag::cells, tag::above, P,
                     tag::of_dim_plus_one, tag::along );
	for ( itt.reset(); itt.in_range(); itt++ )
	{	Cell & sseg = *itt;
		Cell & PP = sseg.base().reverse();
		if ( not PP.belongs_to(interface) )
		// if ( PP.meshes[1]->find(&interface) == PP.meshes[1]->end() )
			layer.insert (&PP);                                             }
	} // intermediate block of code for hiding variables
	}	// outer block of code for hiding variables
	for ( short int i = 0; i < 3; i++ )
	for ( set<Cell*>::iterator itt = layer.begin(); itt != layer.end(); itt++ )
	{	Cell & P = * (*itt);
		double xx = 0., yy = 0.;
		if ( P.belongs_to(bdry) )
		// if ( P.meshes[1]->find(&bdry) != P.meshes[1]->end() )
		{	// baricenter on the boundary
			// if ( ( &P != &A ) and ( &P != &B ) and ( &P != &C ) and ( &P != &D ) )
			if ( corners.find(&P) == corners.end() )
			{	Cell & seg1 = bdry.cell_in_front_of(P);
				Cell & Q = seg1.tip();
				Cell & seg2 = bdry.cell_behind(P);
				Cell & R = seg2.base().reverse();
				xx = ( x(Q) + x(R) ) / 2.;
				yy = ( y(Q) + y(R) ) / 2.;                }                            }
		else  // baricenter inside 'ambient'
		{	double counter = 0.;
			CellIterator itg = ambient.iter_over
				( tag::cells, tag::above, P, tag::of_dim_plus_one, tag::around );
			for ( itg.reset(); itg.in_range(); itg++ )
			{	Cell & seg = *itg;  Cell & Q = seg.base().reverse();
				xx += x(Q);  yy += y(Q);  counter += 1.;   }
			xx /= counter; yy /= counter;                               }
		FunctionOnMesh::prescribe_on (P); x == xx; y == yy;                               }

	// cut wide angles
	{	// just a block of code for hiding variables
	CellIterator it = interface.iter_over ( tag::segments, tag::along );
	Cell * last_seg;
	for ( it.reset(); it.in_range(); it++ )
	{	Cell & seg = *it; last_seg = &seg;
		analyse_angles_right ( ambient, interface, seg );
		analyse_angles_left ( ambient, interface, seg );
	} // end of for it
	Cell & seg = last_seg->reverse();
	// como se fosse o primeiro segmento da interface revertida
	analyse_angles_right ( ambient, interface, seg );
	analyse_angles_left ( ambient, interface, seg );
	}	// just a block of code for hiding variables
	
	// apply baricenter for the first layer of neighbours
	for ( short int i = 0; i < 3; i++ )
	for ( set<Cell*>::iterator itt = layer.begin(); itt != layer.end(); itt++ )
	{	Cell & P = * (*itt);
		double xx = 0., yy = 0.;
		if ( not P.belongs_to(bdry) )
		// if ( P.meshes[1]->find(&bdry) == P.meshes[1]->end() )
		{	double counter = 0.;
			CellIterator itg = ambient.iter_over
				( tag::cells, tag::above, P, tag::of_dim_plus_one, tag::around );
			for ( itg.reset(); itg.in_range(); itg++ )
			{	Cell & seg = *itg;  Cell & Q = seg.base().reverse();
				xx += x(Q);  yy += y(Q);  counter += 1.;   }
			xx /= counter; yy /= counter;
			FunctionOnMesh::prescribe_on (P); x == xx; y == yy;                   }  }

	return interface;

} // end of build_interface


void hanging_nodes ( Mesh & square, FunctionOnMesh::Function & psi, short int layers )

// splits square cells in four, near the level set  psi == 0
// 'layers' tells us how many squares we want to be split
// layers == 0  means only those where psi changes sign will be split
// layers == 1  means we also split their immediate neighbours
// layers == 2  means one more layer of neighbours, and so on
	
{	set < Cell * > set_of_sq;

	// look for cells where psi changes sign :
	
	{ // just a block of code for hiding 'it'
	CellIterator it = square.iter_over ( tag::cells, tag::of_max_dim, tag::oriented );
	for ( it.reset(); it.in_range(); it++ )
	{	bool psi_changes_sign = false;
		Cell & sq = *it;
		CellIterator itt = sq.boundary().iter_over ( tag::segments, tag::around );
		for ( itt.reset(); itt.in_range(); itt++ )
		{	Cell & seg = *itt;
			Cell & P = seg.base().reverse();
			Cell & Q = seg.tip();
			if ( opposite_signs ( psi(P), psi(Q) ) )
			{	psi_changes_sign = true; break;  }          }
		// inserir tambem os 4 vizinhos
		if ( psi_changes_sign ) set_of_sq.insert (&sq);                                 }
	} // just a block of code for hiding 'it'

	// add layers of neighbours :
	
	set < Cell * > set_of_sq_2; // neighbours of set_of_sq
	for ( short int i = 0; i < layers; i++ )
	{	set_of_sq_2.clear();
		set<Cell*>::iterator it;
		for ( it = set_of_sq.begin(); it != set_of_sq.end(); it++ )
		{	Cell * sq = *it;
			CellIterator itt = sq->boundary().iter_over ( tag::segments, tag::around );
			for ( itt.reset(); itt.in_range(); itt++ )
			{	Cell & face = *itt;
				Cell * neigh = square.cell_in_front_of ( face, tag::may_not_exist );
				if ( neigh ) set_of_sq_2.insert ( neigh );                             }   }
		set<Cell*>::iterator it_2;
		for ( it_2 = set_of_sq_2.begin(); it_2 != set_of_sq_2.end(); it_2++ )
			set_of_sq.insert ( *it_2 );                                                       }

	// finally, split each cell in four smaller rectangles :
	
	{ // just a block of code for hiding 'it'
	set<Cell*>::iterator it;
	for ( it = set_of_sq.begin(); it != set_of_sq.end(); it++ )
	{	Cell * sq = *it;
		sq->split ( tag::in_four_rectangles, tag::cell_is_rectangle, tag::within, square );  }
	} // just a block of code for hiding 'it'

} // end of hanging_nodes

int main ()
	
{	Manifold RR2 = Manifold::euclid (2);
	FunctionOnMesh::Function & xy = RR2.coordinate_system ("Lagrange degree one");
	FunctionOnMesh::Function & x = xy[0], & y = xy[1];

	auto & A = Cell::point();  x == -1.;  y == 0.;
	auto & B = Cell::point();  x == 1.;  y == 0.;
	auto & C = Cell::point();  x == 1.;  y == 1.;
	auto & D = Cell::point();  x == -1.;  y == 1.;

	auto & AB = Mesh::segment ( A, B, 24 );
	auto & BC = Mesh::segment ( B, C, 12 );
	auto & CD = Mesh::segment ( C, D, 24 );
	auto & DA = Mesh::segment ( D, A, 12 );
	auto & bdry = Mesh::join ( AB, BC, CD, DA );
	// auto & bdry = Mesh::join ( list<Mesh*>{&AB,&BC,&CD,&DA} );
	set < Cell * > corners { &A, &B, &C, &D };

	auto & rect_mesh = Mesh::rectangle ( AB, BC, CD, DA );
	// rect_mesh.draw_ps ("square.eps");

	double radius = 0.35;
	FunctionOnMesh::Function psi = 0.5 * ( ( x*x + (y-0.2)*(y-0.2) ) / radius - radius );

	// 0.5 * ( ( x*x + (y-0.2)*(y-0.2) ) / radius - radius )  circulo

	// x*x + (y-0.2)*(y-0.2) - 0.3 + 0.2*x*y - 1.35*x*x*y*y
	// x*x + (y-0.2)*(y-0.2) - 0.3 + 0.2*x*y - 1.5*x*x*y*y
	
	hanging_nodes ( rect_mesh, psi, 1 );
	hanging_nodes ( rect_mesh, psi, 1 );
	// splits rectangular cells in four, near the level set psi == 0
	// third argument controls how many layers of cells we want to be split
	// (how far from the level set)

	Mesh & cut = build_interface ( rect_mesh, bdry, corners, psi );
	// deforms the mesh near the level set psi == 0, splits some rectangles in two triangles

	special_draw ( rect_mesh, cut, "square-cut.eps" );
	// rect_mesh.draw_ps ("square-cut.eps");
		
	cout << "reached end" << endl;
	
} // end of main

