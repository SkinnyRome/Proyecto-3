/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
/* Original version Copyright (C) Scott Bilas, 2000.
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Bilas, 2000"
 */
#ifndef _SINGLETON_H__
#define _SINGLETON_H__

// Added by Steve Streeting for Ogre
#include "OgrePrerequisites.h"

#if OGRE_COMPILER == OGRE_COMPILER_MSVC
#   pragma warning (push)
#   pragma warning ( disable: 4661)
#endif

#if defined ( OGRE_GCC_VISIBILITY )
#   pragma GCC visibility push(default)
#endif
namespace Ogre {
    /** \addtogroup Core
    *  @{
    */
    /** \addtogroup General
    *  @{
    */

// End SJS additions
    /** Template class for creating single-instance global classes.
    */
    template <typename T> class Singleton
    {
    private:
        /** @brief Explicit private copy constructor. This is a forbidden operation.*/
        Singleton(const Singleton<T> &);

        /** @brief Private operator= . This is a forbidden operation. */
        Singleton& operator=(const Singleton<T> &);
    
    protected:

        static T* msSingleton;

    public:
        Singleton( void )
        {
            assert( !msSingleton );
#if defined( _MSC_VER ) && _MSC_VER < 1200   
            int offset = (int)(T*)1 - (int)(Singleton <T>*)(T*)1;
            msSingleton = (T*)((int)this + offset);
#else
        msSingleton = static_cast< T* >( this );
#endif
        }
        ~Singleton( void )
            {  assert( msSingleton );  msSingleton = 0;  }
        /// Get the singleton instance
        static T& getSingleton( void )
        {   assert( msSingleton );  return ( *msSingleton ); }
        /// @copydoc getSingleton
        static T* getSingletonPtr( void )
        { return msSingleton; }
    };
    /** @} */
    /** @} */

}
#if OGRE_COMPILER == OGRE_COMPILER_MSVC
#   pragma warning (pop)
#endif
#if defined ( OGRE_GCC_VISIBILITY )
#   pragma GCC visibility pop
#endif

#endif
