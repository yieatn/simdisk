//
// Created by yieatn on 2021/9/15.
//

#ifndef SIMDISK_INODE_H
#define SIMDISK_INODE_H

#include <regex>

const int BLOCKSIZE = 1024;

const int SIMDISKSIZE = 100 << 20;

const int FREEBLOCKS = 1024;
//bit map uses 12.5 blocks`
const int INDEXNODE = FREEBLOCKS + (13 << 10);
//inode used 1 MiB
const int ROOTDIR = INDEXNODE + (1 << 20);

const uint16_t IS_DIRECTORY = 0x0200;


struct inode{
    uint16_t i_mode;
    uint16_t i_uid;
    uint32_t i_size;
    uint32_t i_time;
    uint32_t i_zone[13];
};

const uint INODENUM = (1<<20) / (sizeof(inode));

inline inode inodes[INODENUM];

struct dir_entry{
    uint inode_id;
    //only support file name for 28 characters
    char name[32-sizeof(int)];
};

inline uint64_t bitmap[1600];

inline std::fstream DISK("../disk_inode", std::ios::in | std::ios::out | std::ios::binary);

uint getNewInode() {
    for (uint i = 0; i < INODENUM; ++i) {
        if (inodes[i].i_size == 0)
            return i;
    }
    return -1;
}

uint32_t getNewEmptyBlockNo() {
    uint i;
    for (i = 0; i < (sizeof(bitmap) / sizeof(*bitmap)) && 0xFFFFFFFFFFFFFFFF == bitmap[i]; ++i);
    const int bitInWord = ffsll(~bitmap[i]);
    if (bitInWord != 0)
        bitmap[i] |= (1 << (bitInWord - 1));
    const uint32_t firstZeroBit = bitInWord ? i * sizeof(*bitmap) * 8 + bitInWord : 0;
    return firstZeroBit;
}

std::vector<std::string> split(const std::string& s,
                               const std::regex& re = std::regex("[^\\s \"]+|\"([^\"]*)\"")) {
    std::vector<std::string> cmds;
    for_each(std::sregex_iterator(s.begin(), s.end(), re),
             std::sregex_iterator(),
             [&](const auto& match) {
                 int p = 0;
                 while (match[p].length()) p++;
                 cmds.push_back(match[p - 1]);
             });
    return cmds;
}

#endif //SIMDISK_INODE_H