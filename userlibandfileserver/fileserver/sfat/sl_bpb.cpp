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
// f32\sfat\sl_bpb.cpp
// Boot sector code, specific for EFat.fsy
// 
//


/**
 @file
 @internalTechnology
*/


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!
//!! WARNING!! DO NOT edit this file !! '\sfat' component is obsolete and is not being used. '\sfat32'replaces it
//!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#include "sl_std.h"

//-------------------------------------------------------------------------------------------------------------------

TFatBootSector::TFatBootSector()
    {
    Initialise();    
    }

/** initialises the boot sector data */
void TFatBootSector::Initialise()
    {
    Mem::FillZ(this, sizeof(TFatBootSector));
    }

//-------------------------------------------------------------------------------------------------------------------

/**
    @return ETrue if the boot sector contents seems to be valid
*/
TBool TFatBootSector::IsValid() const
    {
    const TFatType fatType = FatType();
   
    
    const TUint32 totSectors = Max(TotalSectors(), HugeSectors());
    const TUint32 rootDirStartSec =  ReservedSectors() + FatSectors()*NumberOfFats(); //-- root directory start sector

    if(fatType == EInvalid || ReservedSectors() < 1 || NumberOfFats() < 1 || FatSectors() < 1 || rootDirStartSec < 3 ||
       RootDirEntries() < 1 || totSectors < 5)
        goto Invalid;

    if(TotalSectors() >0 && HugeSectors() >0 )
        goto Invalid; //-- values clash

    return ETrue;
  
    Invalid:
        __PRINT(_L("TFatBootSector::IsValid() failed!"));

    return EFalse;
    }

//-------------------------------------------------------------------------------------------------------------------

/**
    Initialize boot sector object from the given bufer. Does not validate the data.
    @param  aBuf buffer with data.
*/
void TFatBootSector::Internalize(const TDesC8& aBuf)
    {
    ASSERT(aBuf.Size() >= KSizeOfFatBootSector);

    Initialise();
    
    TInt pos=0;

    Mem::Copy(&iJumpInstruction, &aBuf[pos],3);     pos+=3; // 0    TUint8 iJumpInstruction[3]
    Mem::Copy(&iVendorId,&aBuf[pos],KVendorIdSize); pos+=KVendorIdSize; // 3    TUint8 iVendorId[KVendorIdSize]
    Mem::Copy(&iBytesPerSector,&aBuf[pos],2);       pos+=2; // 11   TUint16 iBytesPerSector
    Mem::Copy(&iSectorsPerCluster,&aBuf[pos],1);    pos+=1; // 13   TUint8 iSectorsPerCluster   
    Mem::Copy(&iReservedSectors,&aBuf[pos],2);      pos+=2; // 14   TUint16 iReservedSectors
    Mem::Copy(&iNumberOfFats,&aBuf[pos],1);         pos+=1; // 16   TUint8 iNumberOfFats
    Mem::Copy(&iRootDirEntries,&aBuf[pos],2);       pos+=2; // 17   TUint16 iRootDirEntries
    Mem::Copy(&iTotalSectors,&aBuf[pos],2);         pos+=2; // 19   TUint16 iTotalSectors
    Mem::Copy(&iMediaDescriptor,&aBuf[pos],1);      pos+=1; // 21   TUint8 iMediaDescriptor
    Mem::Copy(&iFatSectors,&aBuf[pos],2);           pos+=2; // 22   TUint16 iFatSectors
    Mem::Copy(&iSectorsPerTrack,&aBuf[pos],2);      pos+=2; // 24   TUint16 iSectorsPerTrack
    Mem::Copy(&iNumberOfHeads,&aBuf[pos],2);        pos+=2; // 26   TUint16 iNumberOfHeads
    Mem::Copy(&iHiddenSectors,&aBuf[pos],4);        pos+=4; // 28   TUint32 iHiddenSectors
    Mem::Copy(&iHugeSectors,&aBuf[pos],4);          pos+=4; // 32   TUint32 iHugeSectors
    Mem::Copy(&iPhysicalDriveNumber,&aBuf[pos],1);  pos+=1;// 36|64 TUint8 iPhysicalDriveNumber
    Mem::Copy(&iReserved,&aBuf[pos],1);             pos+=1;// 37|65 TUint8 iReserved
    Mem::Copy(&iExtendedBootSignature,&aBuf[pos],1);pos+=1;// 38|66 TUint8 iExtendedBootSignature
    Mem::Copy(&iUniqueID,&aBuf[pos],4);             pos+=4;// 39|67 TUint32 iUniqueID
    Mem::Copy(&iVolumeLabel,&aBuf[pos],KVolumeLabelSize);  // 43|71 TUint8 iVolumeLabel[KVolumeLabelSize]
    pos+=KVolumeLabelSize;

    // 54|82    TUint8 iFileSysType[KFileSysTypeSize]
    ASSERT(aBuf.Size() >= pos+KFileSysTypeSize);
    Mem::Copy(&iFileSysType,&aBuf[pos],KFileSysTypeSize);
    }

//-------------------------------------------------------------------------------------------------------------------

/**
    Externalize boot sector object to the given data buffer.
    @param  aBuf buffer to externalize.
*/
void TFatBootSector::Externalize(TDes8& aBuf) const
    {
    ASSERT(aBuf.MaxSize() >= KSizeOfFatBootSector);

    if(aBuf.Size() < KSizeOfFatBootSector)
        aBuf.SetLength(KSizeOfFatBootSector);

    TInt pos=0;

    Mem::Copy(&aBuf[pos],&iJumpInstruction,3);      pos+=3;
    Mem::Copy(&aBuf[pos],&iVendorId,KVendorIdSize); pos+=8;
    Mem::Copy(&aBuf[pos],&iBytesPerSector,2);       pos+=2;
    Mem::Copy(&aBuf[pos],&iSectorsPerCluster,1);    pos+=1;
    Mem::Copy(&aBuf[pos],&iReservedSectors,2);      pos+=2;
    Mem::Copy(&aBuf[pos],&iNumberOfFats,1);         pos+=1;
    Mem::Copy(&aBuf[pos],&iRootDirEntries,2);       pos+=2;
    Mem::Copy(&aBuf[pos],&iTotalSectors,2);         pos+=2;
    Mem::Copy(&aBuf[pos],&iMediaDescriptor,1);      pos+=1;
    Mem::Copy(&aBuf[pos],&iFatSectors,2);           pos+=2;
    Mem::Copy(&aBuf[pos],&iSectorsPerTrack,2);      pos+=2;
    Mem::Copy(&aBuf[pos],&iNumberOfHeads,2);        pos+=2;
    Mem::Copy(&aBuf[pos],&iHiddenSectors,4);        pos+=4;
    Mem::Copy(&aBuf[pos],&iHugeSectors,4);          pos+=4;
    Mem::Copy(&aBuf[pos],&iPhysicalDriveNumber,1);  pos+=1;
    Mem::FillZ(&aBuf[pos],1);                       pos+=1;
    Mem::Copy(&aBuf[pos],&iExtendedBootSignature,1);pos+=1;
    Mem::Copy(&aBuf[pos],&iUniqueID,4);             pos+=4;
    
    Mem::Copy(&aBuf[pos],&iVolumeLabel,KVolumeLabelSize); 
    pos+=KVolumeLabelSize;
    
    ASSERT(aBuf.MaxSize() >= pos+KFileSysTypeSize);
    Mem::Copy(&aBuf[pos],&iFileSysType,KFileSysTypeSize);
    }

//-------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
/** replaces all non-printable characters in a buffer with spaces */
static void FixDes(TDes& aDes)
{
    for(TInt i=0; i< aDes.Length(); ++i)
    {
        TChar ch=aDes[i];
        if(!ch.IsPrint())
            aDes[i]=' ';    
    }
}
#endif

/** 
    Print out the boot sector info.
*/
void TFatBootSector::PrintDebugInfo() const
    {
#ifdef _DEBUG
    __PRINT(_L("\n======== BootSector info: =======\n"));
    
    TBuf<40> buf;
    buf.Copy(FileSysType()); FixDes(buf);    
    __PRINT1(_L("FAT type:%S"), &buf);

    buf.Copy(VendorId()); FixDes(buf);    
    __PRINT1(_L("Vendor ID:%S"), &buf);

    __PRINT1(_L("BytesPerSector:%d"),BytesPerSector());
    __PRINT1(_L("SectorsPerCluster:%d"),SectorsPerCluster());
    __PRINT1(_L("ReservedSectors:%d"),ReservedSectors());
    __PRINT1(_L("NumberOfFats:%d"),NumberOfFats());
    __PRINT1(_L("RootDirEntries:%d"),RootDirEntries());
    __PRINT1(_L("Total Sectors:%d"),TotalSectors());
    __PRINT1(_L("MediaDescriptor:0x%x"),MediaDescriptor());
    __PRINT1(_L("FatSectors:%d"),FatSectors());
    __PRINT1(_L("SectorsPerTrack:%d"),SectorsPerTrack());
    __PRINT1(_L("NumberOfHeads:%d"),NumberOfHeads());
    __PRINT1(_L("HugeSectors:%d"),HugeSectors());
    __PRINT1(_L("Root Cluster Number:%d"),RootClusterNum());
    __PRINT1(_L("FSInfo Sector Number:%d"),FSInfoSectorNum());
    __PRINT1(_L("Backup Boot Rec Sector Number:%d"),BkBootRecSector());
    __PRINT1(_L("PhysicalDriveNumber:%d"),PhysicalDriveNumber());
    __PRINT1(_L("ExtendedBootSignature:%d"),ExtendedBootSignature());
    __PRINT1(_L("UniqueID:0x%x"),UniqueID());
    
    buf.Copy(VolumeLabel()); FixDes(buf);    
    __PRINT1(_L("VolumeLabel:%S"), &buf);
    
    __PRINT(_L("=============================\n"));
#endif

    }

//-------------------------------------------------------------------------------------------------------------------

/**
    Determine FAT type according to the information from boot sector, see FAT32 specs.
    @return  FAT type. 
*/
TFatType TFatBootSector::FatType(void) const
    {
    //-- check iBytesPerSector validity; it shall be one of: 512,1024,2048,4096
    if(!IsPowerOf2(iBytesPerSector) || iBytesPerSector < 512 ||  iBytesPerSector > 4096)
        return EInvalid; //-- invalid iBytesPerSector value

    //-- check iSectorsPerCluster validity, it shall be one of: 1,2,4,8...128
    if(!IsPowerOf2(iSectorsPerCluster) || iSectorsPerCluster > 128)
        return EInvalid; //-- invalid iSectorsPerCluster value


    const TUint32 rootDirSectors = (iRootDirEntries*KSizeOfFatDirEntry + (iBytesPerSector-1)) / iBytesPerSector;
    const TUint32 totSec = iTotalSectors ? iTotalSectors : iHugeSectors;
    const TUint32 dataSec = totSec - (iReservedSectors + (iNumberOfFats * iFatSectors) + rootDirSectors);
    const TUint32 clusterCnt = dataSec / iSectorsPerCluster;

    //-- magic. see FAT specs for details.
    if(clusterCnt < 4085)
        return EFat12;
    else if(clusterCnt < 65525)
        return EFat16;
    else
        return EInvalid; //-- FAT32 is not supported by this fsy
    }

//-------------------------------------------------------------------------------------------------------------------

/** @return The first Fat sector number */
TInt TFatBootSector::FirstFatSector() const
    {
    __ASSERT_DEBUG(IsValid(), Fault(EFatBadBootSectorParameter));
    return ReservedSectors();
    }

/**
    @return Number of sectors in root directory. 0 for FAT32
*/
TUint32 TFatBootSector::RootDirSectors() const
    {
    __ASSERT_DEBUG(IsValid(), Fault(EFatBadBootSectorParameter));
    return ( (RootDirEntries()*KSizeOfFatDirEntry + (BytesPerSector()-1)) / BytesPerSector() );
    }


/** @return Start sector number of the root directory */
TInt TFatBootSector::RootDirStartSector()  const
    {
    __ASSERT_DEBUG(IsValid(), Fault(EFatBadBootSectorParameter));

    //-- FAT12/16 root dir starts just after the FATs
    return ReservedSectors() + TotalFatSectors()*NumberOfFats();
    }


/** @return first data sector number. for FAT32 it includes the root directory */
TInt TFatBootSector::FirstDataSector() const
    {
    return( ReservedSectors() + NumberOfFats()*TotalFatSectors() + RootDirSectors() );
    }

/** @return FAT-type independent sector count on the volume */
TUint32 TFatBootSector::VolumeTotalSectorNumber() const
    {
    __ASSERT_DEBUG(IsValid(), Fault(EFatBadBootSectorParameter));
    return TotalSectors() >0 ? (TUint32)TotalSectors() : (TUint32)HugeSectors();
    }

/** @return FAT-type independent number of sectors in one FAT */
TUint32 TFatBootSector::TotalFatSectors() const
    {
    __ASSERT_DEBUG(IsValid(), Fault(EFatBadBootSectorParameter));
    return (TUint32)FatSectors();
    }






















