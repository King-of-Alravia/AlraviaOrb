// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef ALRAVIAORB_TXDB_LEVELDB_H
#define ALRAVIAORB_TXDB_LEVELDB_H

#include "main.h"
#include "leveldb.h"

/** COrbsView backed by the LevelDB coin database (chainstate/) */
class COrbsViewDB : public COrbsView
{
protected:
    CLevelDB db;
public:
    COrbsViewDB(size_t nCacheSize, bool fMemory = false, bool fWipe = false);

    bool GetOrbs(const uint256 &txid, COrbs &orbs);
    bool SetOrbs(const uint256 &txid, const COrbs &orbs);
    bool HaveOrbs(const uint256 &txid);
    CBlockIndex *GetBestBlock();
    bool SetBestBlock(CBlockIndex *pindex);
    bool BatchWrite(const std::map<uint256, COrbs> &mapOrbs, CBlockIndex *pindex);
    bool GetStats(COrbsStats &stats);
};

/** Access to the block database (blocks/index/) */
class CBlockTreeDB : public CLevelDB
{
public:
    CBlockTreeDB(size_t nCacheSize, bool fMemory = false, bool fWipe = false);
private:
    CBlockTreeDB(const CBlockTreeDB&);
    void operator=(const CBlockTreeDB&);
public:
    bool WriteBlockIndex(const CDiskBlockIndex& blockindex);
    bool ReadBestInvalidWork(CBigNum& bnBestInvalidWork);
    bool WriteBestInvalidWork(const CBigNum& bnBestInvalidWork);
    bool ReadBlockFileInfo(int nFile, CBlockFileInfo &fileinfo);
    bool WriteBlockFileInfo(int nFile, const CBlockFileInfo &fileinfo);
    bool ReadLastBlockFile(int &nFile);
    bool WriteLastBlockFile(int nFile);
    bool WriteReindexing(bool fReindex);
    bool ReadReindexing(bool &fReindex);
    bool ReadTxIndex(const uint256 &txid, CDiskTxPos &pos);
    bool WriteTxIndex(const std::vector<std::pair<uint256, CDiskTxPos> > &list);
    bool WriteFlag(const std::string &name, bool fValue);
    bool ReadFlag(const std::string &name, bool &fValue);
    bool LoadBlockIndexGuts();
};

#endif // ALRAVIAORB_TXDB_LEVELDB_H
