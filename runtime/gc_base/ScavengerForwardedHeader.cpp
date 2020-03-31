/*******************************************************************************
 * Copyright (c) 1991, 2019 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at https://www.eclipse.org/legal/epl-2.0/
 * or the Apache License, Version 2.0 which accompanies this distribution and
 * is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following
 * Secondary Licenses when the conditions for such availability set
 * forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
 * General Public License, version 2 with the GNU Classpath
 * Exception [1] and GNU General Public License, version 2 with the
 * OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/

#include "ScavengerForwardedHeader.hpp"

#include "AtomicOperations.hpp"
#include "HeapLinkedFreeHeader.hpp"
#include "ObjectModel.hpp"
#include "SlotObject.hpp"


/**
 * Update this object to be forwarded to destinationObjectPtr using atomic operations.
 * If the update fails (because the object has already been forwarded), read the forwarded
 * header and return the forwarded object which was written into the header.
 * 
 * @parm[in] destinationObjectPtr the object to forward to
 * 
 * @return the winning forwarded object (either destinationObjectPtr or one written by another thread)
 */
omrobjectptr_t
MM_ScavengerForwardedHeader::setForwardedObject(omrobjectptr_t destinationObjectPtr)
{
	Assert_MM_false(isForwardedPointer());

	UDATA oldValue = _preserved;
	UDATA newValue = (UDATA)destinationObjectPtr | FORWARDED_TAG;

#if defined(OMR_GC_COMPRESSED_POINTERS) && !defined(J9VM_ENV_LITTLE_ENDIAN)
	if (compressObjectReferences()) {
		/* The tag bits are in the low bits of newValue. In order to have those tags
		 * appear in the class slot of the object header, the pointer must be
		 * endian-flipped.
		 *
		 * A similar flip will be required when reading the forwarded pointer from the header
		 * (see MM_ScavengerForwardedHeader::getForwardedObjectNoCheck).
		 */
		newValue = (newValue >> 32) | (newValue << 32);
	}
#endif /* defined(OMR_GC_COMPRESSED_POINTERS) && !defined(J9VM_ENV_LITTLE_ENDIAN) */

	if (MM_AtomicOperations::lockCompareExchange((volatile UDATA*)_objectPtr, oldValue, newValue) != oldValue) {
		MM_ScavengerForwardedHeader forwardedObject(_objectPtr, compressObjectReferences());
		destinationObjectPtr = forwardedObject.getForwardedObjectNoCheck();
	}

	return destinationObjectPtr;
}

#if defined(J9VM_GC_VLHGC)
omrobjectptr_t
MM_ScavengerForwardedHeader::setForwardedObjectGrowing(omrobjectptr_t destinationObjectPtr, bool isObjectGrowing)
{
	omrobjectptr_t growthTaggedDestinationPtr = destinationObjectPtr;

	/* no tags must be set on the incoming pointer */
	Assert_MM_true(0 == ((UDATA)destinationObjectPtr & ALL_TAGS));
	if (isObjectGrowing) {
		growthTaggedDestinationPtr = (omrobjectptr_t)((UDATA)growthTaggedDestinationPtr | GROW_TAG);
	}
	return (omrobjectptr_t)((UDATA)setForwardedObject(growthTaggedDestinationPtr) & ~GROW_TAG);
}

bool
MM_ScavengerForwardedHeader::didObjectGrowOnCopy()
{
	/* this only applies to forwarded objects */
	Assert_MM_true(isForwardedPointer());
	return (GROW_TAG == (getPreservedClassAndTags() & GROW_TAG));
}
#endif /* defined(J9VM_GC_VLHGC) */
