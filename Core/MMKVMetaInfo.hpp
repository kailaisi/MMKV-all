/*
 * Tencent is pleased to support the open source community by making
 * MMKV available.
 *
 * Copyright (C) 2018 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MMKV_MMKVMETAINFO_H
#define MMKV_MMKVMETAINFO_H
#ifdef __cplusplus

#include "aes/AESCrypt.h"
#include <cstdint>
#include <cstring>

namespace mmkv {

enum MMKVVersion : uint32_t {
    //默认状态
    MMKVVersionDefault = 0,

    //记录完整的写入次数
    // record full write back count
    MMKVVersionSequence = 1,

    // store random iv for encryption
    //对随机数据进行加密存储
    MMKVVersionRandomIV = 2,

    // store actual size together with crc checksum, try to reduce file corruption
    //保存实际的crc和存储所占用的大小。
    MMKVVersionActualSize = 3,
};

struct MMKVMetaInfo {
    uint32_t m_crcDigest = 0;//crc校验位
    uint32_t m_version = MMKVVersionSequence;//MMKV当前的状态。枚举类型MMKVVersion
    uint32_t m_sequence = 0; // full write-back count
    uint8_t m_vector[AES_KEY_LEN] = {};//aes加密密钥
    uint32_t m_actualSize = 0;//实际大小

    // confirmed info: it's been synced to file
    //已经同步到文件的数据
    struct {
        uint32_t lastActualSize = 0;
        uint32_t lastCRCDigest = 0;
        uint32_t _reserved[16] = {};
    } m_lastConfirmedMetaInfo;

    void write(void *ptr) const {
        MMKV_ASSERT(ptr);
        memcpy(ptr, this, sizeof(MMKVMetaInfo));
    }

    void writeCRCAndActualSizeOnly(void *ptr) const {
        MMKV_ASSERT(ptr);
        auto other = (MMKVMetaInfo *) ptr;
        other->m_crcDigest = m_crcDigest;
        other->m_actualSize = m_actualSize;
    }

    void read(const void *ptr) {
        MMKV_ASSERT(ptr);
        memcpy(this, ptr, sizeof(MMKVMetaInfo));
    }
};

static_assert(sizeof(MMKVMetaInfo) <= (4 * 1024), "MMKVMetaInfo lager than one pagesize");

} // namespace mmkv

#endif
#endif //MMKV_MMKVMETAINFO_H
