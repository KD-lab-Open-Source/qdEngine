//
//  Copyright (c) 2002-2003
//  Toon Knapen, Kresimir Fresl, Joerg Walter
//
//  Permission to use, copy, modify, distribute and sell this software
//  and its documentation for any purpose is hereby granted without fee,
//  provided that the above copyright notice appear in all copies and
//  that both that copyright notice and this permission notice appear
//  in supporting documentation.  The authors make no representations
//  about the suitability of this software for any purpose.
//  It is provided "as is" without express or implied warranty.
//
//

#ifndef BOOST_UBLAS_RAW_H
#define BOOST_UBLAS_RAW_H

namespace boost { namespace numeric { namespace ublas {

// MSVC 6.0 gets confused by the forward declarations.
#ifndef BOOST_MSVC

    template < typename V >
    BOOST_UBLAS_INLINE
    int size( const V &v ) ;

    template < typename V >
    BOOST_UBLAS_INLINE
    int size( const vector_reference<V> &v ) ;

    template < typename M >
    BOOST_UBLAS_INLINE
    int size1( const M &m ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    int size2( const M &m ) ;

    template < typename M >
    BOOST_UBLAS_INLINE
    int size1( const matrix_reference<M> &m ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    int size2( const matrix_reference<M> &m ) ;

    template < typename M >
    BOOST_UBLAS_INLINE
    int leading_dimension( const M &m, row_major_tag ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    int leading_dimension( const M &m, column_major_tag ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    int leading_dimension( const M &m ) ;

    template < typename M >
    BOOST_UBLAS_INLINE
    int leading_dimension( const matrix_reference<M> &m ) ;

    template < typename V >
    BOOST_UBLAS_INLINE
    int stride( const V &v ) ;

    template < typename V >
    BOOST_UBLAS_INLINE
    int stride( const vector_range<V> &v ) ;
    template < typename V >
    BOOST_UBLAS_INLINE
    int stride( const vector_slice<V> &v ) ;

    template < typename M >
    BOOST_UBLAS_INLINE
    int stride( const matrix_row<M> &v ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    int stride( const matrix_column<M> &v ) ;

    template < typename M >
    BOOST_UBLAS_INLINE
    int stride1( const M &m ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    int stride2( const M &m ) ;

    template < typename M >
    BOOST_UBLAS_INLINE
    int stride1( const matrix_reference<M> &m ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    int stride2( const matrix_reference<M> &m ) ;

    template < typename T, std::size_t M, std::size_t N >
    BOOST_UBLAS_INLINE
    int stride1( const c_matrix<T, M, N> &m ) ;
    template < typename T, std::size_t M, std::size_t N >
    BOOST_UBLAS_INLINE
    int stride2( const c_matrix<T, M, N> &m ) ;

    template < typename M >
    BOOST_UBLAS_INLINE
    int stride1( const matrix_range<M> &m ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    int stride1( const matrix_slice<M> &m ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    int stride2( const matrix_range<M> &m ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    int stride2( const matrix_slice<M> &m ) ;

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename MV >
    BOOST_UBLAS_INLINE
    typename MV::const_pointer data( const MV &mv ) ;
#endif
    // We need data_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename MV >
    BOOST_UBLAS_INLINE
    typename MV::const_pointer data_const( const MV &mv ) ;
    template < typename MV >
    BOOST_UBLAS_INLINE
    typename MV::pointer data( MV &mv ) ;

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::const_pointer data( const vector_reference<V> &v ) ;
#endif
    // We need data_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::const_pointer data_const( const vector_reference<V> &v ) ;
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::pointer data( vector_reference<V> &v ) ;

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename T, std::size_t N >
    BOOST_UBLAS_INLINE
    typename c_vector<T, N>::const_pointer data( const c_vector<T, N> &v ) ;
#endif
#ifndef BOOST_MSVC
    // We need data_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename T, std::size_t N >
    BOOST_UBLAS_INLINE
    typename c_vector<T, N>::const_pointer data_const( const c_vector<T, N> &v ) ;
    template < typename T, std::size_t N >
    BOOST_UBLAS_INLINE
    typename c_vector<T, N>::pointer data( c_vector<T, N> &v ) ;
#endif

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::const_pointer data( const vector_range<V> &v ) ;
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::const_pointer data( const vector_slice<V> &v ) ;
#endif
    // We need data_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::const_pointer data_const( const vector_range<V> &v ) ;
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::const_pointer data_const( const vector_slice<V> &v ) ;
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::pointer data( vector_range<V> &v ) ;
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::pointer data( vector_slice<V> &v ) ;

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer data( const matrix_reference<M> &m ) ;
#endif
    // We need data_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer data_const( const matrix_reference<M> &m ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::pointer data( matrix_reference<M> &m ) ;

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename T, std::size_t M, std::size_t N >
    BOOST_UBLAS_INLINE
    typename c_matrix<T, M, N>::const_pointer data( const c_matrix<T, M, N> &m ) ;
#endif
#ifndef BOOST_MSVC
    // We need data_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename T, std::size_t M, std::size_t N >
    BOOST_UBLAS_INLINE
    typename c_matrix<T, M, N>::const_pointer data_const( const c_matrix<T, M, N> &m ) ;
    template < typename T, std::size_t M, std::size_t N >
    BOOST_UBLAS_INLINE
    typename c_matrix<T, M, N>::pointer data( c_matrix<T, M, N> &m ) ;
#endif

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer data( const matrix_row<M> &v ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer data( const matrix_column<M> &v ) ;
#endif
    // We need data_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer data_const( const matrix_row<M> &v ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer data_const( const matrix_column<M> &v ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::pointer data( matrix_row<M> &v ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::pointer data( matrix_column<M> &v ) ;

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer data( const matrix_range<M> &m ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer data( const matrix_slice<M> &m ) ;
#endif
    // We need data_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer data_const( const matrix_range<M> &m ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer data_const( const matrix_slice<M> &m ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::pointer data( matrix_range<M> &m ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::pointer data( matrix_slice<M> &m ) ;

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename MV >
    BOOST_UBLAS_INLINE
    typename MV::const_pointer base( const MV &mv ) ;
#endif
    // We need base_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename MV >
    BOOST_UBLAS_INLINE
    typename MV::const_pointer base_const( const MV &mv ) ;
    template < typename MV >
    BOOST_UBLAS_INLINE
    typename MV::pointer base( MV &mv ) ;

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::const_pointer base( const vector_reference<V> &v ) ;
#endif
    // We need base_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::const_pointer base_const( const vector_reference<V> &v ) ;
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::pointer base( vector_reference<V> &v ) ;

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename T, std::size_t N >
    BOOST_UBLAS_INLINE
    typename c_vector<T, N>::const_pointer base( const c_vector<T, N> &v ) ;
#endif
#ifndef BOOST_MSVC
    // We need base_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename T, std::size_t N >
    BOOST_UBLAS_INLINE
    typename c_vector<T, N>::const_pointer base_const( const c_vector<T, N> &v ) ;
    template < typename T, std::size_t N >
    BOOST_UBLAS_INLINE
    typename c_vector<T, N>::pointer base( c_vector<T, N> &v ) ;
#endif

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::const_pointer base( const vector_range<V> &v ) ;
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::const_pointer base( const vector_slice<V> &v ) ;
#endif
    // We need base_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::const_pointer base_const( const vector_range<V> &v ) ;
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::const_pointer base_const( const vector_slice<V> &v ) ;
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::pointer base( vector_range<V> &v ) ;
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::pointer base( vector_slice<V> &v ) ;

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer base( const matrix_reference<M> &m ) ;
#endif
    // We need base_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer base_const( const matrix_reference<M> &m ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::pointer base( matrix_reference<M> &m ) ;

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename T, std::size_t M, std::size_t N >
    BOOST_UBLAS_INLINE
    typename c_matrix<T, M, N>::const_pointer base( const c_matrix<T, M, N> &m ) ;
#endif
#ifndef BOOST_MSVC
    // We need base_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename T, std::size_t M, std::size_t N >
    BOOST_UBLAS_INLINE
    typename c_matrix<T, M, N>::const_pointer base_const( const c_matrix<T, M, N> &m ) ;
    template < typename T, std::size_t M, std::size_t N >
    BOOST_UBLAS_INLINE
    typename c_matrix<T, M, N>::pointer base( c_matrix<T, M, N> &m ) ;
#endif

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer base( const matrix_row<M> &v ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer base( const matrix_column<M> &v ) ;
#endif
    // We need base_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer base_const( const matrix_row<M> &v ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer base_const( const matrix_column<M> &v ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::pointer base( matrix_row<M> &v ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::pointer base( matrix_column<M> &v ) ;

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer base( const matrix_range<M> &m ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer base( const matrix_slice<M> &m ) ;
#endif
    // We need base_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer base_const( const matrix_range<M> &m ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer base_const( const matrix_slice<M> &m ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::pointer base( matrix_range<M> &m ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::pointer base( matrix_slice<M> &m ) ;

    template < typename MV >
    BOOST_UBLAS_INLINE
    typename MV::size_type start( const MV &mv ) ;

    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::size_type start( const vector_range<V> &v ) ;
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::size_type start( const vector_slice<V> &v ) ;

    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::size_type start( const matrix_row<M> &v ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::size_type start( const matrix_column<M> &v ) ;

    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::size_type start( const matrix_range<M> &m ) ;
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::size_type start( const matrix_slice<M> &m ) ;

#endif



    template < typename V >
    BOOST_UBLAS_INLINE
    int size( const V &v ) {
        return v.size() ;
    }

    template < typename V >
    BOOST_UBLAS_INLINE
    int size( const vector_reference<V> &v ) {
        return size( v ) ;
    }

    template < typename M >
    BOOST_UBLAS_INLINE
    int size1( const M &m ) {
        return m.size1() ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    int size2( const M &m ) {
        return m.size2() ;
    }

    template < typename M >
    BOOST_UBLAS_INLINE
    int size1( const matrix_reference<M> &m ) {
        return size1( m.expression() ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    int size2( const matrix_reference<M> &m ) {
        return size2( m.expression() ) ;
    }

    template < typename M >
    BOOST_UBLAS_INLINE
    int leading_dimension( const M &m, row_major_tag ) {
        return m.size2() ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    int leading_dimension( const M &m, column_major_tag ) {
        return m.size1() ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    int leading_dimension( const M &m ) {
        return leading_dimension( m, BOOST_UBLAS_TYPENAME M::orientation_category() ) ;
    }

    template < typename M >
    BOOST_UBLAS_INLINE
    int leading_dimension( const matrix_reference<M> &m ) {
        return leading_dimension( m.expression() ) ;
    }

    template < typename V >
    BOOST_UBLAS_INLINE
    int stride( const V &v ) {
        return 1 ;
    }

    template < typename V >
    BOOST_UBLAS_INLINE
    int stride( const vector_range<V> &v ) {
        return stride( v.data() ) ;
    }
    template < typename V >
    BOOST_UBLAS_INLINE
    int stride( const vector_slice<V> &v ) {
        return v.stride() * stride( v.data() ) ;
    }

    template < typename M >
    BOOST_UBLAS_INLINE
    int stride( const matrix_row<M> &v ) {
        return stride2( v.data() ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    int stride( const matrix_column<M> &v ) {
        return stride1( v.data() ) ;
    }

    template < typename M >
    BOOST_UBLAS_INLINE
    int stride1( const M &m ) {
        typedef typename M::functor_type functor_type;
        return functor_type::one1( m.size1(), m.size2() ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    int stride2( const M &m ) {
        typedef typename M::functor_type functor_type;
        return functor_type::one2( m.size1(), m.size2() ) ;
    }

    template < typename M >
    BOOST_UBLAS_INLINE
    int stride1( const matrix_reference<M> &m ) {
        return stride1( m.expression() ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    int stride2( const matrix_reference<M> &m ) {
        return stride2( m.expression() ) ;
    }

    template < typename T, std::size_t M, std::size_t N >
    BOOST_UBLAS_INLINE
    int stride1( const c_matrix<T, M, N> &m ) {
        return N ;
    }
    template < typename T, std::size_t M, std::size_t N >
    BOOST_UBLAS_INLINE
    int stride2( const c_matrix<T, M, N> &m ) {
        return 1 ;
    }

    template < typename M >
    BOOST_UBLAS_INLINE
    int stride1( const matrix_range<M> &m ) {
        return stride1( m.data() ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    int stride1( const matrix_slice<M> &m ) {
        return m.stride1() * stride1( m.data() ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    int stride2( const matrix_range<M> &m ) {
        return stride2( m.data() ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    int stride2( const matrix_slice<M> &m ) {
        return m.stride2() * stride2( m.data() ) ;
    }

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename MV >
    BOOST_UBLAS_INLINE
    typename MV::const_pointer data( const MV &mv ) {
        return &mv.data().begin()[0] ;
    }
#endif
    // We need data_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename MV >
    BOOST_UBLAS_INLINE
    typename MV::const_pointer data_const( const MV &mv ) {
        return &mv.data().begin()[0] ;
    }
    template < typename MV >
    BOOST_UBLAS_INLINE
    typename MV::pointer data( MV &mv ) {
        return &mv.data().begin()[0] ;
    }

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::const_pointer data( const vector_reference<V> &v ) {
        return data( v.expression () ) ;
    }
#endif
    // We need data_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::const_pointer data_const( const vector_reference<V> &v ) {
        return data_const( v.expression () ) ;
    }
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::pointer data( vector_reference<V> &v ) {
        return data( v.expression () ) ;
    }

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename T, std::size_t N >
    BOOST_UBLAS_INLINE
    typename c_vector<T, N>::const_pointer data( const c_vector<T, N> &v ) {
        return v.data() ;
    }
#endif
#ifndef BOOST_MSVC
    // We need data_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename T, std::size_t N >
    BOOST_UBLAS_INLINE
    typename c_vector<T, N>::const_pointer data_const( const c_vector<T, N> &v ) {
        return v.data() ;
    }
    template < typename T, std::size_t N >
    BOOST_UBLAS_INLINE
    typename c_vector<T, N>::pointer data( c_vector<T, N> &v ) {
        return v.data() ;
    }
#endif

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::const_pointer data( const vector_range<V> &v ) {
        return data( v.data() ) + v.start() * stride (v.data() ) ;
    }
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::const_pointer data( const vector_slice<V> &v ) {
        return data( v.data() ) + v.start() * stride (v.data() ) ;
    }
#endif
    // We need data_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::const_pointer data_const( const vector_range<V> &v ) {
        return data_const( v.data() ) + v.start() * stride (v.data() ) ;
    }
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::const_pointer data_const( const vector_slice<V> &v ) {
        return data_const( v.data() ) + v.start() * stride (v.data() ) ;
    }
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::pointer data( vector_range<V> &v ) {
        return data( v.data() ) + v.start() * stride (v.data() ) ;
    }
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::pointer data( vector_slice<V> &v ) {
        return data( v.data() ) + v.start() * stride (v.data() ) ;
    }

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer data( const matrix_reference<M> &m ) {
        return data( m.expression () ) ;
    }
#endif
    // We need data_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer data_const( const matrix_reference<M> &m ) {
        return data_const( m.expression () ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::pointer data( matrix_reference<M> &m ) {
        return data( m.expression () ) ;
    }

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename T, std::size_t M, std::size_t N >
    BOOST_UBLAS_INLINE
    typename c_matrix<T, M, N>::const_pointer data( const c_matrix<T, M, N> &m ) {
        return m.data() ;
    }
#endif
#ifndef BOOST_MSVC
    // We need data_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename T, std::size_t M, std::size_t N >
    BOOST_UBLAS_INLINE
    typename c_matrix<T, M, N>::const_pointer data_const( const c_matrix<T, M, N> &m ) {
        return m.data() ;
    }
    template < typename T, std::size_t M, std::size_t N >
    BOOST_UBLAS_INLINE
    typename c_matrix<T, M, N>::pointer data( c_matrix<T, M, N> &m ) {
        return m.data() ;
    }
#endif

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer data( const matrix_row<M> &v ) {
        return data( v.data() ) + v.index() * stride1( v.data() ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer data( const matrix_column<M> &v ) {
        return data( v.data() ) + v.index() * stride2( v.data() ) ;
    }
#endif
    // We need data_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer data_const( const matrix_row<M> &v ) {
        return data_const( v.data() ) + v.index() * stride1( v.data() ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer data_const( const matrix_column<M> &v ) {
        return data_const( v.data() ) + v.index() * stride2( v.data() ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::pointer data( matrix_row<M> &v ) {
        return data( v.data() ) + v.index() * stride1( v.data() ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::pointer data( matrix_column<M> &v ) {
        return data( v.data() ) + v.index() * stride2( v.data() ) ;
    }

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer data( const matrix_range<M> &m ) {
        return data( m.data() ) + m.start1() * stride1( m.data () ) + m.start2() * stride2( m.data () ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer data( const matrix_slice<M> &m ) {
        return data( m.data() ) + m.start1() * stride1( m.data () ) + m.start2() * stride2( m.data () ) ;
    }
#endif
    // We need data_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer data_const( const matrix_range<M> &m ) {
        return data_const( m.data() ) + m.start1() * stride1( m.data () ) + m.start2() * stride2( m.data () ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer data_const( const matrix_slice<M> &m ) {
        return data_const( m.data() ) + m.start1() * stride1( m.data () ) + m.start2() * stride2( m.data () ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::pointer data( matrix_range<M> &m ) {
        return data( m.data() ) + m.start1() * stride1( m.data () ) + m.start2() * stride2( m.data () ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::pointer data( matrix_slice<M> &m ) {
        return data( m.data() ) + m.start1() * stride1( m.data () ) + m.start2() * stride2( m.data () ) ;
    }

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename MV >
    BOOST_UBLAS_INLINE
    typename MV::const_pointer base( const MV &mv ) {
        return &mv.data().begin()[0] ;
    }
#endif
    // We need base_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename MV >
    BOOST_UBLAS_INLINE
    typename MV::const_pointer base_const( const MV &mv ) {
        return &mv.data().begin()[0] ;
    }
    template < typename MV >
    BOOST_UBLAS_INLINE
    typename MV::pointer base( MV &mv ) {
        return &mv.data().begin()[0] ;
    }

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::const_pointer base( const vector_reference<V> &v ) {
        return base( v.expression () ) ;
    }
#endif
    // We need base_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::const_pointer base_const( const vector_reference<V> &v ) {
        return base_const( v.expression () ) ;
    }
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::pointer base( vector_reference<V> &v ) {
        return base( v.expression () ) ;
    }

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename T, std::size_t N >
    BOOST_UBLAS_INLINE
    typename c_vector<T, N>::const_pointer base( const c_vector<T, N> &v ) {
        return v.data() ;
    }
#endif
#ifndef BOOST_MSVC
    // We need base_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename T, std::size_t N >
    BOOST_UBLAS_INLINE
    typename c_vector<T, N>::const_pointer base_const( const c_vector<T, N> &v ) {
        return v.data() ;
    }
    template < typename T, std::size_t N >
    BOOST_UBLAS_INLINE
    typename c_vector<T, N>::pointer base( c_vector<T, N> &v ) {
        return v.data() ;
    }
#endif

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::const_pointer base( const vector_range<V> &v ) {
        return base( v.data() ) ;
    }
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::const_pointer base( const vector_slice<V> &v ) {
        return base( v.data() ) ;
    }
#endif
    // We need base_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::const_pointer base_const( const vector_range<V> &v ) {
        return base_const( v.data() ) ;
    }
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::const_pointer base_const( const vector_slice<V> &v ) {
        return base_const( v.data() ) ;
    }
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::pointer base( vector_range<V> &v ) {
        return base( v.data() ) ;
    }
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::pointer base( vector_slice<V> &v ) {
        return base( v.data() ) ;
    }

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer base( const matrix_reference<M> &m ) {
        return base( m.expression () ) ;
    }
#endif
    // We need base_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer base_const( const matrix_reference<M> &m ) {
        return base_const( m.expression () ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::pointer base( matrix_reference<M> &m ) {
        return base( m.expression () ) ;
    }

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename T, std::size_t M, std::size_t N >
    BOOST_UBLAS_INLINE
    typename c_matrix<T, M, N>::const_pointer base( const c_matrix<T, M, N> &m ) {
        return m.data() ;
    }
#endif
#ifndef BOOST_MSVC
    // We need base_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename T, std::size_t M, std::size_t N >
    BOOST_UBLAS_INLINE
    typename c_matrix<T, M, N>::const_pointer base_const( const c_matrix<T, M, N> &m ) {
        return m.data() ;
    }
    template < typename T, std::size_t M, std::size_t N >
    BOOST_UBLAS_INLINE
    typename c_matrix<T, M, N>::pointer base( c_matrix<T, M, N> &m ) {
        return m.data() ;
    }
#endif

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer base( const matrix_row<M> &v ) {
        return base( v.data() ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer base( const matrix_column<M> &v ) {
        return base( v.data() ) ;
    }
#endif
    // We need base_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer base_const( const matrix_row<M> &v ) {
        return base_const( v.data() ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer base_const( const matrix_column<M> &v ) {
        return base_const( v.data() ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::pointer base( matrix_row<M> &v ) {
        return base( v.data() ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::pointer base( matrix_column<M> &v ) {
        return base( v.data() ) ;
    }

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer base( const matrix_range<M> &m ) {
        return base( m.data() ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer base( const matrix_slice<M> &m ) {
        return base( m.data() ) ;
    }
#endif
    // We need base_const() mostly due to MSVC 6.0.
    // But how shall we write portable code otherwise?
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer base_const( const matrix_range<M> &m ) {
        return base_const( m.data() ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::const_pointer base_const( const matrix_slice<M> &m ) {
        return base_const( m.data() ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::pointer base( matrix_range<M> &m ) {
        return base( m.data() ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::pointer base( matrix_slice<M> &m ) {
        return base( m.data() ) ;
    }

    template < typename MV >
    BOOST_UBLAS_INLINE
    typename MV::size_type start( const MV &mv ) {
        return 0 ;
    }

    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::size_type start( const vector_range<V> &v ) {
        return v.start() * stride (v.data() ) ;
    }
    template < typename V >
    BOOST_UBLAS_INLINE
    typename V::size_type start( const vector_slice<V> &v ) {
        return v.start() * stride (v.data() ) ;
    }

    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::size_type start( const matrix_row<M> &v ) {
        return v.index() * stride1( v.data() ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::size_type start( const matrix_column<M> &v ) {
        return v.index() * stride2( v.data() ) ;
    }

    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::size_type start( const matrix_range<M> &m ) {
        return m.start1() * stride1( m.data () ) + m.start2() * stride2( m.data () ) ;
    }
    template < typename M >
    BOOST_UBLAS_INLINE
    typename M::size_type start( const matrix_slice<M> &m ) {
        return m.start1() * stride1( m.data () ) + m.start2() * stride2( m.data () ) ;
    }

}}}

#endif
