void CellIterator::AroundCell::OfCodimTwo::OverSegments::NormalOrder::reset ( )

// virtual from CellIterator::Core

// we know (due to tag::inner) that we are not on the boundary of the mesh
// so any cell "above" the center will do (as initial segment)
// it's like a closed loop
	
{	assert ( this->msh_p );
	assert ( cen );
	assert ( cen->get_dim() + 3 == this->msh_p->get_dim_plus_one() );
	if ( cen->is_positive() ) this->center = cen;
	else this->center = cen->reverse_attr.core;
	assert ( this->center );
	assert ( this->center->is_positive() );
	Cell::Positive * pos_cen = tag::Util::assert_cast
		< Cell::Core*, Cell::Positive* > ( this->center );

	this->current_segment = find_first_seg ( pos_cen, this->msh_p );
	assert ( this->current_segment );

	this->current_vertex = nullptr;
	// this is useless if the iterator is fresh (current_vertex is initialized as nullptr)
	// but the user may want to reset a previously used iterator

	Cell this_center ( tag::whose_core_is, this->center,
	                   tag::previously_existing, tag::surely_not_null );
	this->last_vertex = this->current_segment->boundary().cell_in_front_of
		( this_center, tag::surely_exists ).reverse().core;                    }

// the above could get simpler and faster by using a method like
// inline Cell::Core * Mesh::Core::cell_behind_ptr
// ( const Cell::Core * face, const tag::MayNotExist & ) const
// which is not difficult to implement


void CellIterator::AroundCell::OfCodimTwo::OverSegments::ReverseOrder::reset
( const tag::Center &, Cell::Core * const cen )

// virtual from CellIterator::Core

// we don't know whether we are in the interior or on the boundary of the mesh
// so we must search for the first_ver, rotating forward
	
{	assert ( this->msh_p );
	assert ( cen );
	assert ( cen->get_dim() + 3 == this->msh_p->get_dim_plus_one() );
	this->current_vertex = nullptr;
	if ( cen->is_positive() ) this->center = cen;
	else this->center = cen->reverse_attr.core;
	assert ( this->center );
	assert ( this->center->is_positive() );
	Cell::Positive * pos_cen = tag::Util::assert_cast
		< Cell::Core*, Cell::Positive* > ( this->center );

	this->current_segment = find_first_seg ( pos_cen, this->msh_p );
	assert ( this->current_segment );

	this->current_vertex = nullptr;
	// this is useless if the iterator is fresh (current_vertex is initialized as nullptr)
	// but the user may want to reset a previously used iterator

	// now we rotate backwards until we meet the boundary
	rotate_forward_for_seg ( this );                                   }

// the above could get simpler and faster by using a method like
// inline Cell::Core * Mesh::Core::cell_behind_ptr
// ( const Cell::Core * face, const tag::MayNotExist & ) const
// which is not difficult to implement


void CellIterator::AroundCell::OfCodimTwo::OverSegments::ReverseOrder::reset
( const tag::Center &, Cell::Core * const cen, const tag::Inner & )

// virtual from CellIterator::Core

// we know (due to tag::inner) that we are not on the boundary of the mesh
// so any cell "above" the center will do (as initial segment)
// it's like a closed loop
	
{	assert ( this->msh_p );
	assert ( cen );
	assert ( cen->get_dim() + 3 == this->msh_p->get_dim_plus_one() );
	if ( cen->is_positive() ) this->center = cen;
	else this->center = cen->reverse_attr.core;
	assert ( this->center );
	assert ( this->center->is_positive() );
	Cell::Positive * pos_cen = tag::Util::assert_cast
		< Cell::Core*, Cell::Positive* > ( this->center );

	this->current_segment = find_first_seg ( pos_cen, this->msh_p );
	assert ( this->current_segment );

	this->current_vertex = nullptr;
	// this is useless if the iterator is fresh (current_vertex is initialized as nullptr)
	// but the user may want to reset a previously used iterator

	Cell this_center ( tag::whose_core_is, this->center,
	                   tag::previously_existing, tag::surely_not_null );
	this->last_vertex = this->current_segment->boundary().cell_behind
		( this_center, tag::surely_exists ).reverse().core;                    }

// the above could get simpler and faster by using a method like
// inline Cell::Core * Mesh::Core::cell_behind_ptr
// ( const Cell::Core * face, const tag::MayNotExist & ) const
// which is not difficult to implement



//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------


class CellIterator::AroundCell::OfCodimTwo::OverSegments
: public CellIterator::AroundCell::OfCodimTwo::Ordered

// when cll->dim == msh->dim - 2, there is a linear order
// e.g. we rotate around a vertex in a 2D mesh
// or we rotate around a segment in a 3D mesh

// the dimension of the cells to be returned is a different matter
// e.g. we can return segments or squares (or cubes) around a vertex
// or squares (or cubes) around a segment

// 'Segments' means we want to get cells of dimension center->dim + 2 == msh->dim
// the procedure is analogous to CellIterator::Over::SegmentsOfConnectedOneDimMesh

// here, words "vertex" and "segment" are quite misleading
// they come from an analogy to iterators over chains of segments
// "vertex" means a cell of dimension center->dim + 1 == msh->dim - 1
// "segment" means a cell of dimension center->dim + 2 == msh->dim
// when we rotate around a vertex, "vertex" means segment, "segment" means square
// when we rotate around a segment, "vertex" means square, "segment" means cube

{	public :

	// attributes inherited from CellIterator::AroundCell :
	// Mesh::Connected::OneDim * msh_p
	// Cell::Positive * center
	
	// attributes inherited from CellIterator::AroundCell::OfCodimTwo::Ordered :
	// Cell::Core * current_segment { nullptr }  -- word "segment" is misleading here
	// Cell::Core * current_vertex { nullptr }  -- word "vertex" is misleading here

	Cell::Core * last_vertex { nullptr };  // word "vertex" is misleading here

	inline OverSegments ( Mesh::Core * msh, Cell::Positive * c )
	:	CellIterator::AroundCell::OfCodimTwo::Ordered ( msh, c )  // msh_p { msh }, center { c }
	{ }
	
	// void reset ( )  stays pure virtual from CellIterator::Core

	// void reset ( const tag::StartAt &, Cell::Core * cll )
	//   virtual, defined by CellIterator::AroundCell, execution forbidden
	
	// Cell deref ( )  stays pure virtual from CellIterator::Core
	// void advance ( )  stays pure virtual from CellIterator::Core

	bool in_range ( );  // virtual from CellIterator::Core
	
	class NormalOrder;  class ReverseOrder;

};  // end of class CellIterator::AroundCell::OfCodimTwo::OverSegments

//-----------------------------------------------------------------------------------------


class CellIterator::AroundCell::OfCodimTwo::OverSegments::NormalOrder
: public CellIterator::AroundCell::OfCodimTwo::OverSegments

// when cll->dim == msh->dim - 2, there is a linear order
// e.g. we rotate around a vertex in a 2D mesh
// or we rotate around a segment in a 3D mesh
// here we follow this order

// the dimension of the cells to be returned is a different matter
// e.g. we can return segments or squares around a vertex
// or squares or cubes around a segment

{	public :

	// attributes inherited from CellIterator::AroundCell :
	// Mesh::Connected::OneDim * msh_p
	// Cell::Positive * center
	
	// attributes inherited from CellIterator::AroundCell::OfCodimTwo::Ordered :
	// Cell::Core * current_segment { nullptr }  -- word "segment" is misleading here
	// Cell::Core * current_vertex { nullptr }  -- word "vertex" is misleading here

	// attribute inherited from CellIterator::AroundCell::OfCodimTwo::OverSegments :
	// Cell::Core * last_vertex { nullptr }  -- word "vertex" is misleading here

	inline NormalOrder ( Mesh::Core * msh, Cell::Positive * c )
	:	CellIterator::AroundCell::OfCodimTwo::OverSegments ( msh, c )
	// msh_p { msh }, center { c }
	{ }
	
	// void reset ( )
	//   virtual, defined by CellIterator::AroundCell, execution forbidden

	// void reset ( const tag::StartAt &, Cell::Core * cll )
	//   virtual, defined by CellIterator::AroundCell, execution forbidden
	
	// Cell deref ( )  stays pure virtual from CellIterator::Core

	void advance ( );  // virtual from CellIterator::Core
	
	// bool in_range ( )  virtual, defined by CellIterator::AroundCell::OfCodimTwo::OverSegments
	
	class AsTheyAre;  class ForcePositive;
	struct ReverseEachCell  {  class  AssumeCellsExist;  };

};  // end of class CellIterator::AroundCell::OfCodimTwo::OverSegments::NormalOrder

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------


class CellIterator::AroundCell::OfCodimTwo::OverSegments::ReverseOrder
: public CellIterator::AroundCell::OfCodimTwo::OverSegments

// when cll->dim == msh->dim - 2, there is a linear order
// e.g. we rotate around a vertex in a 2D mesh
// or we rotate around a segment in a 3D mesh
// here we follow the reversed order

// the dimension of the cells to be returned is a different matter
// e.g. we can return segments or squares around a vertex
// or squares or cubes around a segment

{	public :

	// attributes inherited from CellIterator::AroundCell :
	// Mesh::Connected::OneDim * msh_p
	// Cell::Positive * center
	
	// attributes inherited from CellIterator::AroundCell::OfCodimTwo::Ordered :
	// Cell::Core * current_segment { nullptr }  -- word "segment" is misleading here
	// Cell::Core * current_vertex { nullptr }  -- word "vertex" is misleading here

	// attribute inherited from CellIterator::AroundCell::OfCodimTwo::OverSegments :
	// Cell::Core * last_vertex { nullptr }  -- word "vertex" is misleading here

	inline ReverseOrder ( Mesh::Core * msh, Cell::Positive * c )
	:	CellIterator::AroundCell::OfCodimTwo::OverSegments ( msh, c )
	// msh_p { msh }, center { c }
	{ }
	
	// void reset ( )
	//   virtual, defined by CellIterator::AroundCell, execution forbidden

	// void reset ( const tag::StartAt &, Cell::Core * cll )
	//   virtual, defined by CellIterator::AroundCell, execution forbidden
	
	// Cell deref ( )  stays pure virtual from CellIterator::Core

	void advance ( );  // virtual from CellIterator::Core
	
	// bool in_range ( )  virtual, defined by CellIterator::AroundCell::OfCodimTwo::OverSegments
	
	class AsTheyAre;  class ForcePositive;
	struct ReverseEachCell  {  class  AssumeCellsExist;  };

};  // end of class CellIterator::AroundCell::OfCodimTwo::OverSegments::ReverseOrder

//-----------------------------------------------------------------------------------------


class CellIterator::AroundCell::OfCodimTwo::OverSegments::NormalOrder::AsTheyAre
: public CellIterator::AroundCell::OfCodimTwo::OverSegments::NormalOrder

{	public :

	// attributes inherited from CellIterator::AroundCell :
	// Mesh::Connected::OneDim * msh_p
	// Cell::Positive * center
	
	// attributes inherited from CellIterator::AroundCell::OfCodimTwo::Ordered :
	// Cell::Core * current_segment { nullptr }  -- word "segment" is misleading here
	// Cell::Core * current_vertex { nullptr }  -- word "vertex" is misleading here

	// attribute inherited from CellIterator::AroundCell::OfCodimTwo::OverSegments :
	// Cell::Core * last_vertex { nullptr }  -- word "vertex" is misleading here

	inline AsTheyAre ( Mesh::Core * msh, Cell::Positive * c )
	:	CellIterator::AroundCell::OfCodimTwo::OverSegments::NormalOrder ( msh, c )
	// msh_p { msh }, center { c }
	{ }
	
	// void reset ( )
	//   virtual, defined by CellIterator::AroundCell, execution forbidden

	// void reset ( const tag::StartAt &, Cell::Core * cll )
	//   virtual, defined by CellIterator::AroundCell, execution forbidden
	
	Cell deref ( );  // virtual from CellIterator::Core

	// void advance ( )  virtual from CellIterator::Core, defined by
	//   CellIterator::AroundCell::OfCodimTwo::OverSegments::NormalOrder

	// bool in_range ( )  virtual, defined by CellIterator::AroundCell::OfCodimTwo::OverSegments

};  // end of class CellIterator::AroundCell::OfCodimTwo::OverSegments::NormalOrder::AsTheyAre

//-----------------------------------------------------------------------------------------


class CellIterator::AroundCell::OfCodimTwo::OverSegments::ReverseOrder::AsTheyAre
: public CellIterator::AroundCell::OfCodimTwo::OverSegments::ReverseOrder

{	public :

	// attributes inherited from CellIterator::AroundCell :
	// Mesh::Connected::OneDim * msh_p
	// Cell::Positive * center
	
	// attributes inherited from CellIterator::AroundCell::OfCodimTwo::Ordered :
	// Cell::Core * current_segment { nullptr }  -- word "segment" is misleading here
	// Cell::Core * current_vertex { nullptr }  -- word "vertex" is misleading here

	// attribute inherited from CellIterator::AroundCell::OfCodimTwo::OverSegments :
	// Cell::Core * last_vertex { nullptr }  -- word "vertex" is misleading here

	inline AsTheyAre ( Mesh::Core * msh, Cell::Positive * c )
	:	CellIterator::AroundCell::OfCodimTwo::OverSegments::ReverseOrder ( msh, c )
	// msh_p { msh }, center { c }
	{ }
	
	// void reset ( )
	//   virtual, defined by CellIterator::AroundCell, execution forbidden

	// void reset ( const tag::StartAt &, Cell::Core * cll )
	//   virtual, defined by CellIterator::AroundCell, execution forbidden
	
	Cell deref ( );  // virtual from CellIterator::Core

	// void advance ( )  virtual from CellIterator::Core, defined by
	//   CellIterator::AroundCell::OfCodimTwo::OverSegments::NormalOrder

	// bool in_range ( )  virtual, defined by CellIterator::AroundCell::OfCodimTwo::OverSegments
	
};  // end of class CellIterator::AroundCell::OfCodimTwo
    //                 ::OverSegments::ReverseOrder::AsTheyAre

//-----------------------------------------------------------------------------------------


class CellIterator::AroundCell::OfCodimTwo::OverSegments::NormalOrder::ForcePositive
: public CellIterator::AroundCell::OfCodimTwo::OverSegments::NormalOrder

{	public :

	// attributes inherited from CellIterator::AroundCell :
	// Mesh::Connected::OneDim * msh_p
	// Cell::Positive * center
	
	// attributes inherited from CellIterator::AroundCell::OfCodimTwo::Ordered :
	// Cell::Core * current_segment { nullptr }  -- word "segment" is misleading here
	// Cell::Core * current_vertex { nullptr }  -- word "vertex" is misleading here

	// attribute inherited from CellIterator::AroundCell::OfCodimTwo::OverSegments :
	// Cell::Core * last_vertex { nullptr }  -- word "vertex" is misleading here

	inline ForcePositive ( Mesh::Core * msh, Cell::Positive * c )
	:	CellIterator::AroundCell::OfCodimTwo::OverSegments::NormalOrder ( msh, c )
	// msh_p { msh }, center { c }
	{ }
	
	// void reset ( )
	//   virtual, defined by CellIterator::AroundCell, execution forbidden

	// void reset ( const tag::StartAt &, Cell::Core * cll )
	//   virtual, defined by CellIterator::AroundCell, execution forbidden
	
	Cell deref ( );  // virtual from CellIterator::Core

	// void advance ( )  virtual from CellIterator::Core, defined by
	//   CellIterator::AroundCell::OfCodimTwo::OverSegments::NormalOrder

	// bool in_range ( )  virtual, defined by CellIterator::AroundCell::OfCodimTwo::OverSegments
	
};  // end of class CellIterator::AroundCell::OfCodimTwo
    //                  ::OverSegments::NormalOrder::ForcePositive

//-----------------------------------------------------------------------------------------


class CellIterator::AroundCell::OfCodimTwo::OverSegments::ReverseOrder::ForcePositive
: public CellIterator::AroundCell::OfCodimTwo::OverSegments::ReverseOrder

{	public :

	// attributes inherited from CellIterator::AroundCell :
	// Mesh::Connected::OneDim * msh_p
	// Cell::Positive * center
	
	// attributes inherited from CellIterator::AroundCell::OfCodimTwo::Ordered :
	// Cell::Core * current_segment { nullptr }  -- word "segment" is misleading here
	// Cell::Core * current_vertex { nullptr }  -- word "vertex" is misleading here

	// attribute inherited from CellIterator::AroundCell::OfCodimTwo::OverSegments :
	// Cell::Core * last_vertex { nullptr }  -- word "vertex" is misleading here

	inline ForcePositive ( Mesh::Core * msh, Cell::Positive * c )
	:	CellIterator::AroundCell::OfCodimTwo::OverSegments::ReverseOrder ( msh, c )
	// msh_p { msh }, center { c }
	{ }
	
	// void reset ( )
	//   virtual, defined by CellIterator::AroundCell, execution forbidden

	// void reset ( const tag::StartAt &, Cell::Core * cll )
	//   virtual, defined by CellIterator::AroundCell, execution forbidden
	
	Cell deref ( );  // virtual from CellIterator::Core

	// void advance ( )  virtual from CellIterator::Core, defined by
	//   CellIterator::AroundCell::OfCodimTwo::OverSegments::ReverseOrder

	// bool in_range ( )  virtual, defined by CellIterator::AroundCell::OfCodimTwo::OverSegments
	
};  // end of class CellIterator::AroundCell::OfCodimTwo
    //                  ::OverSegments::ReverseOrder::ForcePositive

//-----------------------------------------------------------------------------------------


class CellIterator::AroundCell::OfCodimTwo
         ::OverSegments::NormalOrder::ReverseEachCell::AssumeCellsExist
: public CellIterator::AroundCell::OfCodimTwo::OverSegments::NormalOrder

{	public :

	// attributes inherited from CellIterator::AroundCell :
	// Mesh::Connected::OneDim * msh_p
	// Cell::Positive * center
	
	// attributes inherited from CellIterator::AroundCell::OfCodimTwo::Ordered :
	// Cell::Core * current_segment { nullptr }  -- word "segment" is misleading here
	// Cell::Core * current_vertex { nullptr }  -- word "vertex" is misleading here

	// attribute inherited from CellIterator::AroundCell::OfCodimTwo::OverSegments :
	// Cell::Core * last_vertex { nullptr }  -- word "vertex" is misleading here

	inline AssumeCellsExist ( Mesh::Core * msh, Cell::Positive * c )
	:	CellIterator::AroundCell::OfCodimTwo::OverSegments::NormalOrder ( msh, c )
	// msh_p { msh }, center { c }
	{ }
	
	// void reset ( )
	//   virtual, defined by CellIterator::AroundCell, execution forbidden

	// void reset ( const tag::StartAt &, Cell::Core * cll )
	//   virtual, defined by CellIterator::AroundCell, execution forbidden
	
	Cell deref ( );  // virtual from CellIterator::Core

	// void advance ( )  virtual from CellIterator::Core, defined by
	//   CellIterator::AroundCell::OfCodimTwo::OverSegments::NormalOrder

	// bool in_range ( )  virtual, defined by CellIterator::AroundCell::OfCodimTwo::OverSegments
	
};  // end of class CellIterator::AroundCell::OfCodimTwo
    //                  ::OverSegments::NormalOrder::ReverseEachCell::AssumeCellsExist

//-----------------------------------------------------------------------------------------


class CellIterator::AroundCell::OfCodimTwo
           ::OverSegments::ReverseOrder::ReverseEachCell::AssumeCellsExist
: public CellIterator::AroundCell::OfCodimTwo::OverSegments::ReverseOrder

{	public :

	// attributes inherited from CellIterator::AroundCell :
	// Mesh::Connected::OneDim * msh_p
	// Cell::Positive * center
	
	// attributes inherited from CellIterator::AroundCell::OfCodimTwo::Ordered :
	// Cell::Core * current_segment { nullptr }  -- word "segment" is misleading here
	// Cell::Core * current_vertex { nullptr }  -- word "vertex" is misleading here

	// attribute inherited from CellIterator::AroundCell::OfCodimTwo::OverSegments :
	// Cell::Core * last_vertex { nullptr }  -- word "vertex" is misleading here

	inline AssumeCellsExist ( Mesh::Core * msh, Cell::Positive * c )
	:	CellIterator::AroundCell::OfCodimTwo::OverSegments::ReverseOrder ( msh, c )
	// msh_p { msh }, center { c }
	{ }
	
	// void reset ( )
	//   virtual, defined by CellIterator::AroundCell, execution forbidden

	// void reset ( const tag::StartAt &, Cell::Core * cll )
	//   virtual, defined by CellIterator::AroundCell, execution forbidden
	
	Cell deref ( );  // virtual from CellIterator::Core

	// void advance ( )  virtual from CellIterator::Core, defined by
	//   CellIterator::AroundCell::OfCodimTwo::OverSegments::ReverseOrder

	// bool in_range ( )  virtual, defined by CellIterator::AroundCell::OfCodimTwo::OverSegments
	
};  // end of class CellIterator::AroundCell::OfCodimTwo
    //                  ::OverSegments::ReverseOrder::ReverseEachCell::AssumeCellsExist

//-----------------------------------------------------------------------------------------


Cell CellIterator::AroundCell::OfCodimTwo::OverSegments::NormalOrder::AsTheyAre::deref ( )
// virtual from CellIterator::Core
{	return Cell ( tag::whose_core_is, this->current_segment,
                tag::previously_existing, tag::surely_not_null );  }


Cell CellIterator::AroundCell::OfCodimTwo::OverSegments::ReverseOrder::AsTheyAre::deref ( )
// virtual from CellIterator::Core
{	return Cell ( tag::whose_core_is, this->current_segment,
                tag::previously_existing, tag::surely_not_null );  }


Cell CellIterator::AroundCell::OfCodimTwo::OverSegments::NormalOrder::ForcePositive::deref ( )
// virtual from CellIterator::Core
{	assert ( this->current_segment );
	return this->current_segment->get_positive();  }


Cell CellIterator::AroundCell::OfCodimTwo::OverSegments::ReverseOrder::ForcePositive::deref ( )
// virtual from CellIterator::Core
{	assert ( this->current_segment );
	return this->current_segment->get_positive();  }


Cell CellIterator::AroundCell::OfCodimTwo::OverSegments::
	NormalOrder::ReverseEachCell::AssumeCellsExist::deref ( )
// virtual from CellIterator::Core
{	assert ( this->current_segment );
	assert ( this->current_segment->reverse_attr.exists() );
	return this->current_segment->reverse_attr;              }


void CellIterator::AroundCell::OfCodimTwo::OverSegments::NormalOrder::advance ( )
// virtual from CellIterator::Core
{	Cell this_center ( tag::whose_core_is, this->center,
	                   tag::previously_existing, tag::surely_not_null );
	Mesh m ( tag::whose_core_is, this->msh_p, tag::previously_existing, tag::is_positive );
	assert ( this->current_segment );
	Cell new_ver = this->current_segment->boundary().cell_behind
		( this_center, tag::surely_exists );
	this->current_vertex = new_ver.core;
	this->current_segment = m.cell_in_front_of ( new_ver, tag::may_not_exist ).core;        }
	// if we are near the boundary, current_segment may be nullptr
	// the calling program must check this using the 'in_range' method

// the above could get simpler and faster by avoiding the use of wrappers


void CellIterator::AroundCell::OfCodimTwo::OverSegments::ReverseOrder::advance ( )
// virtual from CellIterator::Core
{	Cell this_center ( tag::whose_core_is, this->center,
	                   tag::previously_existing, tag::surely_not_null );
	Mesh m ( tag::whose_core_is, this->msh_p, tag::previously_existing, tag::is_positive );
	assert ( this->current_segment );
	Cell new_ver = this->current_segment->boundary().cell_in_front_of
		( this_center, tag::surely_exists );
	this->current_vertex = new_ver.core;
	this->current_segment = m.cell_in_front_of ( new_ver, tag::may_not_exist ).core;        }
	// if we are near the boundary, current_segment may be nullptr
	// the calling program must check this using the 'in_range' method

// the above could get simpler and faster by avoiding the use of wrappers


bool CellIterator::AroundCell::OfCodimTwo::OverSegments::in_range ( )
// virtual from CellIterator::Core  last_vertex
{	if ( this->last_vertex )  // we are not at the boundary of the mesh
	{	if ( this->current_vertex == this->last_vertex )
			// we have completed the loop
			return false;}
	return this->current_segment;         	           }

//-----------------------------------------------------------------------------------------

Cell CellIterator::AroundCell::OfCodimTwo
         ::OverVertices::ReverseOrder::AssumeCellsExist::AsTheyAre::deref ( )
// virtual from CellIterator::Core
{	return Cell ( tag::whose_core_is, this->current_vertex,
                tag::previously_existing, tag::surely_not_null );  }


Cell CellIterator::AroundCell::OfCodimTwo
         ::OverVertices::ReverseOrder::BuildReverseCells::AsTheyAre::deref ( )
// virtual from CellIterator::Core
{	return Cell ( tag::whose_core_is, this->current_vertex,
                tag::previously_existing, tag::surely_not_null );  }


Cell CellIterator::AroundCell
         ::OfCodimTwo::OverVertices::ReverseOrder::AssumeCellsExist::ForcePositive::deref ( )
// virtual from CellIterator::Core
{	assert ( this->current_vertex );
	return this->current_segment->get_positive();  }


Cell CellIterator::AroundCell
         ::OfCodimTwo::OverVertices::ReverseOrder::BuildReverseCells::ForcePositive::deref ( )
// virtual from CellIterator::Core
{	assert ( this->current_vertex );
	return this->current_segment->get_positive();  }


Cell CellIterator::AroundCell::OfCodimTwo::OverVertices::
	ReverseOrder::AssumeCellsExist::ReverseEachCell::deref ( )
// virtual from CellIterator::Core
{	assert ( this->current_vertex );
	assert ( this->current_vertex->reverse_attr.exists() );
	return this->current_vertex->reverse_attr;              }


Cell CellIterator::AroundCell::OfCodimTwo::OverVertices::
	ReverseOrder::BuildReverseCells::ReverseEachCell::deref ( )
// virtual from CellIterator::Core
{	assert ( this->current_vertex );
	if ( not this->current_vertex->reverse_attr.exists() )
	//	this->current_vertex->reverse_attr =
	//		Cell ( tag::whose_core_is, this->current_vertex->build_reverse
	//	         ( tag::one_dummy_wrapper ), tag::freshly_created         );
	{	Cell::Positive * tcv = tag::Util::assert_cast
			< Cell::Core*, Cell::Positive* > ( this->current_vertex );
		tcv->reverse_attr.core = tcv->build_reverse ( tag::one_dummy_wrapper );  }
	assert ( this->current_vertex->reverse_attr.exists() );
	return this->current_vertex->reverse_attr;              }


void CellIterator::AroundCell::OfCodimTwo::OverVertices::ReverseOrder::advance ( )
// virtual from CellIterator::Core
{	Cell this_center ( tag::whose_core_is, this->center,
	                   tag::previously_existing, tag::surely_not_null );
	Mesh m ( tag::whose_core_is, this->msh_p, tag::previously_existing, tag::is_positive );
	assert ( this->current_segment );
	Cell new_ver = this->current_segment->boundary().cell_in_front_of
		( this_center, tag::surely_exists );
	this->current_vertex = new_ver.core;
	this->current_segment = m.cell_in_front_of ( new_ver, tag::may_not_exist ).core;        }
	// if we are near the boundary, current_segment may be nullptr
	// the calling program must check this using the 'in_range' method

// the above could get simpler and faster by avoiding the use of wrappers


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

