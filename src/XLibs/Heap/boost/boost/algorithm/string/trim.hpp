//  Boost string_algo library trim.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#ifndef BOOST_STRING_TRIM_HPP
#define BOOST_STRING_TRIM_HPP

#include <boost/algorithm/string/config.hpp>
#include <boost/algorithm/string/collection_traits.hpp>
#include <boost/algorithm/string/detail/trim.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <locale>

/*! \file
    Defines trim algorithms.
    Trim algorithms are used to remove trailing and leading spaces from a 
    sequence (string). Space is recognized using given locales.

    Parametric (\c _if) variants use predicate (functor) to select which characters
    are to be trimmed.. 
    Functions take a selection predicate as a parameter, which is used to determine 
    if a character is a space. Common predicates are provided in classification.hpp header.

*/

namespace boost {
    namespace algorithm {

    //  left trim  -----------------------------------------------//


        //! Left trim - parametric
        /*!
            Remove all leading spaces from the input. Supplied predicate
            is used to determine which character is a space.
            The result is copied to the given output iterator.

            \param Output A output iterator to which the result will be copied
            \param Input An input collection
            \param IsSpace An unary predicate identifying spaces
            \return An output iterator pointing just after last inserted character
        */
        template<typename OutputIteratorT, typename CollectionT, typename PredicateT>
        inline OutputIteratorT trim_left_copy_if( 
            OutputIteratorT Output,
            const CollectionT& Input,
            PredicateT IsSpace)
        {
            std::copy( 
                detail::trim_begin( 
                    begin(Input), 
                    end(Input), 
                    IsSpace ),
                end(Input),
                Output);

            return Output;
        }

        //! Left trim - parametric
        /*!
            Remove all leading spaces from the input. Supplied predicate
            is used to determine which character is a space.
            The result is a trimmed copy if the input

            \param Input An input sequence
            \param IsSpace An unary predicate identifying spaces
            \return A trimmed copy if the input
        */
        template<typename SequenceT, typename PredicateT>
        inline SequenceT trim_left_copy_if(const SequenceT& Input, PredicateT IsSpace)
        {
            return SequenceT( 
                detail::trim_begin( 
                    begin(Input), 
                    end(Input), 
                    IsSpace ),
                end(Input));
        }

        //! Left trim - parametric
        /*!
            Remove all leading spaces from the input. 
            The result is a trimmed copy if the input

            \param Input An input sequence
            \param Loc a locale used for 'space' classification
            \return A trimmed copy if the input
        */
        template<typename SequenceT>
        inline SequenceT trim_left_copy(const SequenceT& Input, const std::locale& Loc=std::locale())
        {
            return            
                trim_left_copy_if(
                    Input, 
                    is_space(Loc));
        }

        //! Left trim
        /*!
            Remove all leading spaces from the input. Supplied predicate
            is used to determine which character is a space.
            The input sequence is modified in-place.

            \param Input An input sequence
            \param IsSpace An unary predicate identifying spaces
        */
        template<typename SequenceT, typename PredicateT>
        inline void trim_left_if(SequenceT& Input, PredicateT IsSpace)
        {
            Input.erase( 
                begin(Input),
                detail::trim_begin( 
                    begin(Input), 
                    end(Input), 
                    IsSpace));
        }

        //! Left trim
        /*!
            Remove all leading spaces from the input.
            The Input sequence is modified in-place.

            \param Input An input sequence
            \param Loc A locale used for 'space' classification
        */
        template<typename SequenceT>
        inline void trim_left(SequenceT& Input, const std::locale& Loc=std::locale())
        {
            trim_left_if( 
                Input, 
                is_space(Loc));
        }

    //  right trim  -----------------------------------------------//

        //! Right trim - parametric
        /*!
            Remove all trailing spaces from the input. Supplied predicate
            is used to determine which character is a space.
            The result is copied to the given output iterator.

            \param Output A output iterator to which the result will be copied
            \param Input An input collection
            \param IsSpace An unary predicate identifying spaces
            \return An output iterator pointing just after last inserted character
        */
        template<typename OutputIteratorT, typename CollectionT, typename PredicateT>
        inline OutputIteratorT trim_right_copy_if( 
            OutputIteratorT Output,
            const CollectionT& Input,
            PredicateT IsSpace )
        {
            std::copy( 
                begin(Input),
                detail::trim_end( 
                    begin(Input), 
                    end(Input), 
                    IsSpace ),
                Output );

            return Output;
        }

        //! Right trim - parametric
        /*!
            Remove all trailing spaces from the input. Supplied predicate
            is used to determine which character is a space.
            The result is a trimmed copy if the input

            \param Input An input Sequence
            \param IsSpace An unary predicate identifying spaces
            \return A trimmed copy if the input
        */
        template<typename SequenceT, typename PredicateT>
        inline SequenceT trim_right_copy_if(const SequenceT& Input, PredicateT IsSpace)
        {
            return SequenceT( 
                begin(Input),
                detail::trim_end( 
                    begin(Input), 
                    end(Input), 
                    IsSpace)
                );
        }

        //! Right trim
        /*!
            Remove all trailing spaces from the input. 
            The result is a trimmed copy if the input

            \param Input An input sequence
            \param Loc A locale used for 'space' classification
            \return A trimmed copy if the input
        */  
        template<typename SequenceT>
        inline SequenceT trim_right_copy(const SequenceT& Input, const std::locale& Loc=std::locale())
        {
            return 
                trim_right_copy_if( 
                    Input, 
                    is_space(Loc));
        }

            
        //! Right trim - parametric
        /*!
            Remove all trailing spaces from the input. Supplied predicate
            is used to determine which character is a space.
            The input sequence is modified in-place.

            \param Input An input sequence
            \param IsSpace An unary predicate identifying spaces
        */
        template<typename SequenceT, typename PredicateT>
        inline void trim_right_if(SequenceT& Input, PredicateT IsSpace)
        {
            Input.erase(
                detail::trim_end( 
                    begin(Input), 
                    end(Input), 
                    IsSpace ),
                end(Input)
                );
        }


        //! Right trim
        /*!
            Remove all trailing spaces from the input. 
            The input sequence is modified in-place.

            \param Input An input sequence
            \param Loc A locale used for 'space' classification
        */
        template<typename SequenceT>
        inline void trim_right(SequenceT& Input, const std::locale& Loc=std::locale())
        {
            trim_right_if(
                Input, 
                is_space(Loc) );
        }

    //  both side trim  -----------------------------------------------//

        //! Trim - parametric
        /*!
            Remove all trailing and leading spaces from the input. 
            Supplied predicate is used to determine which character is a space.
            The result is copied to the given output iterator.

            \param Output A output iterator to which the result will be copied
            \param Input An input collection
            \param IsSpace An unary predicate identifying spaces
            \return An output iterator pointing just after last inserted character
        */
        template<typename OutputIteratorT, typename CollectionT, typename PredicateT>
        inline OutputIteratorT trim_copy_if( 
            OutputIteratorT Output,
            const CollectionT& Input,
            PredicateT IsSpace)
        {
            BOOST_STRING_TYPENAME 
                const_iterator_of<CollectionT>::type TrimEnd=
                detail::trim_end( 
                    begin(Input), 
                    end(Input), 
                    IsSpace);

            std::copy( 
                detail::trim_begin( 
                    begin(Input), TrimEnd, IsSpace),
                TrimEnd,
                Output
                );

            return Output;
        }

        //! Trim - parametric
        /*!
            Remove all leading and trailing spaces from the input. 
            Supplied predicate is used to determine which character is a space.
            The result is a trimmed copy if the input

            \param Input An input sequence
            \param IsSpace An unary predicate identifying spaces
            \return A trimmed copy if the input
        */
        template<typename SequenceT, typename PredicateT>
        inline SequenceT trim_copy_if(const SequenceT& Input, PredicateT IsSpace)
        {
            BOOST_STRING_TYPENAME 
                const_iterator_of<SequenceT>::type TrimEnd=
                    detail::trim_end( 
                        begin(Input), 
                        end(Input), 
                        IsSpace);

            return SequenceT( 
                detail::trim_begin( 
                    begin(Input), 
                    TrimEnd, 
                    IsSpace),
                TrimEnd
                );
        }

        //! Trim
        /*!
            Remove all leading and trailing spaces from the input. 
            The result is a trimmed copy if the input

            \param Input An input sequence
            \param Loc A locale used for 'space' classification
            \return A trimmed copy if the input
        */
        template<typename SequenceT>
        inline SequenceT trim_copy( const SequenceT& Input, const std::locale& Loc=std::locale() )
        {
            return
                trim_copy_if(
                    Input, 
                    is_space(Loc) );
        }
     
        //! Trim
        /*!
            Remove all leading and trailing spaces from the input. 
            Supplied predicate is used to determine which character is a space.
            The input sequence is modified in-place.

            \param Input An input sequence
            \param IsSpace An unary predicate identifying spaces
        */
        template<typename SequenceT, typename PredicateT>
        inline void trim_if(SequenceT& Input, PredicateT IsSpace)
        {
            trim_right_if( Input, IsSpace );
            trim_left_if( Input, IsSpace );
        }

        //! Trim
        /*!
            Remove all leading and trailing spaces from the input. 
            The input sequence is modified in-place.

            \param Input An input sequence
            \param Loc A locale used for 'space' classification
        */
        template<typename SequenceT>
        inline void trim(SequenceT& Input, const std::locale& Loc=std::locale())
        {
            trim_if(
                Input, 
                is_space( Loc ) );
        }

    } // namespace algorithm 

    // pull names to the boost namespace
    using algorithm::trim_left;
    using algorithm::trim_left_if;
    using algorithm::trim_left_copy;
    using algorithm::trim_left_copy_if;
    using algorithm::trim_right;
    using algorithm::trim_right_if;
    using algorithm::trim_right_copy;
    using algorithm::trim_right_copy_if;
    using algorithm::trim;
    using algorithm::trim_if;
    using algorithm::trim_copy;
    using algorithm::trim_copy_if;

} // namespace boost

#endif  // BOOST_STRING_TRIM_HPP
