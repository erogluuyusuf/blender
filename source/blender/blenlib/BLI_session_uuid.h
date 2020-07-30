/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __BLI_SESSION_UUID_H__
#define __BLI_SESSION_UUID_H__

/** \file
 * \ingroup bli
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "DNA_session_uuid_types.h"

/* Generate new UUID which is unique throughout the Blender session. */
SessionUUID BLI_session_uuid_generate(void);

/* CHeck whether the UUID is properly generated. */
bool BLI_session_uuid_is_generated(const SessionUUID *uuid);

/* Check whether two UUIDs are identical. */
bool BLI_session_uuid_is_equal(const SessionUUID *lhs, const SessionUUID *rhs);

/* Utility functions to make it possible to create GHash/GSet with UUID as a key. */
uint BLI_session_uuid_ghash_hash(const void *uuid_v);
bool BLI_session_uuid_ghash_compare(const void *lhs_v, const void *rhs_v);

#ifdef __cplusplus
}
#endif

#endif /* __BLI_SESSION_UUID_H__ */
