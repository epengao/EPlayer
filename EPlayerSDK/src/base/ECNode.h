/*
 * ****************************************************************
 * This software is a media player SDK implementation
 * GPL:
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details. You should
 * have received a copy of the GNU Library General Public License
 * along with this library; if not, write to the Free Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Project: EC < Enjoyable Coding >
 *
 * ECNode.h
 * This file for basic struct Node type define.
 *
 * Eamil:   epengao@126.com
 * Author:  Gao Peng
 * Version: Intial first version.
 * ****************************************************************
 */

#ifndef EC_NODE_H
#define EC_NODE_H

template<typename T>
class ECNode
{
public:
    ECNode(const T data)
           :m_sData(data)
           ,m_pNext(EC_NULL) {}

public:
    T          m_sData;
    ECNode<T>* m_pNext;
};

template<typename KeyT, typename ValT>
class ECPairNode
{
public:
    ECPairNode() {}
    ECPairNode(KeyT key, ValT val)
              :m_sKey(key), m_sVal(val) {}

public:
    KeyT    m_sKey;
    ValT    m_sVal;
};

#endif /* EC_NODE_H */
