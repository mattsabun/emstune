/************************************************************************************
 * EMStudio - Open Source ECU tuning software                                       *
 * Copyright (C) 2013  Michael Carpenter (malcom2073@gmail.com)                     *
 *                                                                                  *
 * This file is a part of EMStudio                                                  *
 *                                                                                  *
 * EMStudio is free software; you can redistribute it and/or                        *
 * modify it under the terms of the GNU Lesser General Public                       *
 * License as published by the Free Software Foundation, version                    *
 * 2.1 of the License.                                                              *
 *                                                                                  *
 * EMStudio is distributed in the hope that it will be useful,                      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                *
 * Lesser General Public License for more details.                                  *
 *                                                                                  *
 * You should have received a copy of the GNU Lesser General Public                 *
 * License along with this program; if not, write to the Free Software              *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   *
 ************************************************************************************/

#include "memorylocation.h"
#include "QsLog.h"

MemoryLocation::MemoryLocation()
{
	m_parent = 0;
	parent=0;
	hasParent = false;
}
void MemoryLocation::addChild(MemoryLocation *child)
{
	m_childList.append(child);
}
void MemoryLocation::setParent(MemoryLocation *parent)
{
	m_parent = parent;
}
void MemoryLocation::childChanged(MemoryLocation *child,QByteArray data)
{
	unsigned short childinparent=0;
	if (isRam)
	{
		childinparent = child->ramAddress - this->ramAddress;
	}
	else
	{
		childinparent = child->flashAddress - this->flashAddress;
	}
	if (data.size() != child->size)
	{
		QLOG_ERROR() << "Error. Child tried to replace memory location not equal to its own size! Data size: " << data.size() << "Child size:" << child->size;
	}
	m_data.replace(childinparent,data.length(),data);
}
void MemoryLocation::setByteDirty(unsigned short offset)
{
	if (!m_dirty.contains(offset))
	{
		m_dirty.append(offset);
	}
}

void MemoryLocation::setByteClean(unsigned short offset)
{
	if (m_dirty.contains(offset))
	{
		m_dirty.removeOne(offset);
	}
}

bool MemoryLocation::isDirty()
{
	if (m_dirty.size() == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

QByteArray MemoryLocation::data(MemoryLocation *child)
{
	unsigned short childinparent=0;
	if (isRam)
	{
		childinparent = child->ramAddress - this->ramAddress;
	}
	else
	{
		childinparent = child->flashAddress - this->flashAddress;
	}
	return m_data.mid(childinparent,child->size);
}
bool MemoryLocation::isEmpty()
{
	if (this->data().size() == 0)
	{
		return true;
	}
	return false;
}

QByteArray MemoryLocation::data()
{
	if (m_parent)
	{
		return m_parent->data(this);
	}
	else
	{
		return m_data;
	}
}

void MemoryLocation::setData(QByteArray data)
{
	if (m_parent)
	{
		m_parent->childChanged(this,data);
	}
	else
	{
		m_data = data;
	}
}
