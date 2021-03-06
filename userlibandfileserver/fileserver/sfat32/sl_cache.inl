// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// f32\sfat32\inc\sl_cache.inl
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef SL_CACHE_INL
#define SL_CACHE_INL


/**
    @param  aPos media linear position
    @return ETrue if the aPos belongs to this page
*/
TBool CWTCachePage::PosCached(TInt64 aPos) const
    {
    return (iValid && aPos >= iStartPos && aPos < iStartPos+PageSize());
    }


/**
    Calculate data position within cache page relatively to the page beginning.
    @param  aPos media linear position
    @return data offset from the beginning of the page
*/
TUint32 CWTCachePage::PosInCachePage(TInt64 aPos) const 
    { 
    return ((TUint32)aPos - (TUint32)iStartPos) & (PageSize()-1);
    }

/**
    Obtain pointer to the data in the cache page by media inear position
    @param  aPos media linear position
    @return pointer to the data in page buffer
*/
TUint8* CWTCachePage::PtrInCachePage(TInt64 aPos) const 
    { 
    return (TUint8*)iData.Ptr() + PosInCachePage(aPos);
    }

/**
    @return cache page size in bytes
*/
TUint32 CWTCachePage::PageSize() const 
    {
    return iData.Size();
    }  

//---------------------------------------------------------------------------------------------------------------------------------


/**
    Just helper method to calculate cache page start position by given media pos.
    @param  aPos media linear position
    @return rounded-down and aligned position that is the beginning of the cache page
*/
TInt64  CMediaWTCache::CalcPageStartPos(TInt64 aPos) const 
    { 
    ASSERT(aPos >= iCacheBasePos);
    return (((aPos - iCacheBasePos) >> iPageSizeLog2) << iPageSizeLog2) + iCacheBasePos;
    }

/**
    @return cache page size
*/
TUint32 CMediaWTCache::PageSize() const 
    {
    return 1<<iPageSizeLog2;
    }


/**
    Set cache base position at aBasePos
    @param  aBasePos  base position of the cache pages. Affects pages alignment.
*/
void CMediaWTCache::SetCacheBasePos(TInt64 aBasePos)
    {
    iCacheBasePos = aBasePos;
    }



//---------------------------------------------------------------------------------------------------------------------------------


#endif //SL_CACHE_INL




