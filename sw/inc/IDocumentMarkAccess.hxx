/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#pragma once

#include <sal/types.h>
#include "IMark.hxx"
#include <memory>
#include <optional>

class SwPaM;
struct SwPosition;
class SwTextNode;
class SwCursorShell;

namespace sw::mark {
    class SaveBookmark; // FIXME: Ugly: SaveBookmark is a core-internal class, and should not be used in the interface
    class MarkBase;
}

/** Provides access to the marks of a document.
*/
class IDocumentMarkAccess
{
    public:
        enum class MarkType
        {
            UNO_BOOKMARK,
            DDE_BOOKMARK,
            BOOKMARK,
            CROSSREF_HEADING_BOOKMARK,
            CROSSREF_NUMITEM_BOOKMARK,
            ANNOTATIONMARK,
            TEXT_FIELDMARK,
            CHECKBOX_FIELDMARK,
            DROPDOWN_FIELDMARK,
            DATE_FIELDMARK,
            NAVIGATOR_REMINDER
        };

        /** wrapper iterator: wraps iterator of implementation while hiding
            MarkBase class; only IMark instances can be retrieved directly.
         */
        class SW_DLLPUBLIC iterator
        {
            private:
                std::optional<std::vector<::sw::mark::MarkBase*>::const_iterator> m_pIter;

            public:
                // MarkManager implementation needs to get the real iterator
                std::vector<::sw::mark::MarkBase*>::const_iterator const& get() const;

                typedef std::ptrdiff_t difference_type;
                typedef ::sw::mark::IMark* value_type;
                typedef ::sw::mark::IMark* const* pointer;
                typedef ::sw::mark::IMark* const& reference;
                typedef std::random_access_iterator_tag iterator_category;

                iterator();
                iterator(std::vector<::sw::mark::MarkBase*>::const_iterator const& rIter);
                iterator(iterator const& rOther);
                iterator& operator=(iterator const& rOther);
                iterator(iterator && rOther) noexcept;
                iterator& operator=(iterator && rOther) noexcept;

                // FIXME unfortunately there's a requirement on input iterator
                // and forward iterator to return reference, which isn't
                // possible because we have to return a temp value;
                // let's try value_type instead, perhaps it's sufficient,
                // for a const_iterator...
                ::sw::mark::IMark* /*const&*/ operator*() const;
                // nope can't do that :(
                //::sw::mark::IMark* /* const* */ operator->() const;
                iterator& operator++();
                iterator operator++(int);
                bool operator==(iterator const& rOther) const;
                bool operator!=(iterator const& rOther) const;
                iterator& operator--();
                iterator operator--(int);
                iterator& operator+=(difference_type);
                iterator operator+(difference_type) const;
                iterator& operator-=(difference_type);
                iterator operator-(difference_type) const;
                difference_type operator-(iterator const&) const;
                value_type operator[](difference_type) const;
                bool operator<(iterator const& rOther) const;
                bool operator>(iterator const& rOther) const;
                bool operator<=(iterator const& rOther) const;
                bool operator>=(iterator const& rOther) const;
        };

        typedef iterator const_iterator_t;

        /// To avoid recursive calls of deleteMark, the removal of dummy
        /// characters of fieldmarks has to be delayed; this is the baseclass
        /// that can be subclassed for that purpose.
        struct ILazyDeleter { virtual ~ILazyDeleter() { } };

        /** Generates a new mark in the document for a certain selection.

           @param rPaM
           [in] the selection being marked.

           @param rProposedName
           [in] the proposed name of the new mark.

           @param eMark
           [in] the type of the new mark.

           @param eMode
           [in] is the new mark part of a text copy operation

           @returns
           a pointer to the new mark (name might have changed).
        */
        virtual ::sw::mark::IMark* makeMark(const SwPaM& rPaM,
            const OUString& rProposedName,
            MarkType eMark, ::sw::mark::InsertMode eMode,
            SwPosition const* pSepPos = nullptr) = 0;

        virtual sw::mark::IFieldmark* makeFieldBookmark( const SwPaM& rPaM,
            const OUString& rName,
            const OUString& rType,
            SwPosition const* pSepPos = nullptr) = 0;
        virtual sw::mark::IFieldmark* makeNoTextFieldBookmark( const SwPaM& rPaM,
            const OUString& rName,
            const OUString& rType) = 0;

        virtual sw::mark::IMark* makeAnnotationMark(
            const SwPaM& rPaM,
            const OUString& rName ) = 0;

        /** Returns a mark in the document for a paragraph.
            If there is none, a mark will be created.

           @param rTextNode
           [in] the paragraph being marked (a selection over the paragraph is marked)

           @param eMark
           [in] the type of the new mark.

           @returns
           a pointer to the new mark (name might have changed).
        */
        virtual ::sw::mark::IMark* getMarkForTextNode(const SwTextNode& rTextNode,
            MarkType eMark) =0;

        /** Moves an existing mark to a new selection and performs needed updates.
            @param io_pMark
            [in/out] the mark to be moved

            @param rPaM
            [in] new selection to be marked
        */

        virtual void repositionMark(::sw::mark::IMark* io_pMark,
            const SwPaM& rPaM) =0;

        /** Renames an existing Mark, if possible.
            @param io_pMark
            [in/out] the mark to be renamed

            @param rNewName
            [in] new name for the mark

            @returns false, if renaming failed (because the name is already in use)
        */
        virtual bool renameMark(::sw::mark::IMark* io_pMark,
                const OUString& rNewName) =0;

        /** Corrects marks (absolute)
            This method ignores the previous position of the mark in the paragraph

            @param rOldNode
            [in] the node from which nodes should be moved

            @param rNewPos
            [in] new position to which marks will be moved, if nOffset == 0

            @param nOffset
            [in] the offset by which the mark gets positioned of rNewPos
        */
        virtual void correctMarksAbsolute(const SwNode& rOldNode,
            const SwPosition& rNewPos,
            const sal_Int32 nOffset) =0;

        /** Corrects marks (relative)
            This method uses the previous position of the mark in the paragraph as offset

            @param rOldNode
            [in] the node from which nodes should be moved

            @param rNewPos
            [in] new position to which marks from the start of the paragraph will be
                 moved, if nOffset == 0

            @param nOffset
            [in] the offset by which the mark gets positioned of rNewPos in addition to
                 its old position in the paragraph
        */
        virtual void correctMarksRelative(const SwNode& rOldNode,
            const SwPosition& rNewPos,
            const sal_Int32 nOffset) =0;

        /** Deletes marks in a range

            Note: navigator reminders are excluded

        */
        virtual void deleteMarks(
            const SwNode& rStt,
            const SwNode& rEnd,
            std::vector< ::sw::mark::SaveBookmark>* pSaveBkmk, // Ugly: SaveBookmark is core-internal
            std::optional<sal_Int32> oStartContentIdx,
            std::optional<sal_Int32> oEndContentIdx) =0;

        /** Deletes a mark.

            @param ppMark
            [in] an iterator pointing to the Mark to be deleted.
            @param isMoveNodes
            [in] the mark is deleted only temporarily for a node move, do not
                 remove fieldmark chars.
        */
        virtual std::unique_ptr<ILazyDeleter>
            deleteMark(const IDocumentMarkAccess::const_iterator_t& ppMark, bool isMoveNodes) =0;

        /** Deletes a mark.

            @param ppMark
            [in] the name of the mark to be deleted.
        */
        virtual void deleteMark(const ::sw::mark::IMark* const pMark) =0;

        /** Clear (deletes) all marks.
        */
        virtual void clearAllMarks() =0;

        virtual void assureSortedMarkContainers() const = 0;

        /** returns a STL-like random access iterator to the begin of the sequence of marks.
        */
        virtual const_iterator_t getAllMarksBegin() const =0;

        /** returns a STL-like random access iterator to the end of the sequence of marks.
        */
        virtual const_iterator_t getAllMarksEnd() const =0;

        /** returns the number of marks.

            Note: annotation marks are excluded
        */
        virtual sal_Int32 getAllMarksCount() const =0;

        /** Finds a mark by name.

            @param rName
            [in] the name of the mark to find.

            @returns
            an iterator pointing to the mark, or pointing to getAllMarksEnd() if nothing was found.
        */
        virtual const_iterator_t findMark(const OUString& rMark) const =0;

        // interface IBookmarks (BOOKMARK, CROSSREF_NUMITEM_BOOKMARK, CROSSREF_HEADING_BOOKMARK )

        /** check if the selection would delete a BOOKMARK */
        virtual bool isBookmarkDeleted(SwPaM const& rPaM, bool isReplace) const =0;

        /** returns a STL-like random access iterator to the begin of the sequence the IBookmarks.
        */
        virtual const_iterator_t getBookmarksBegin() const =0;

        /** returns a STL-like random access iterator to the end of the sequence of IBookmarks.
        */
        virtual const_iterator_t getBookmarksEnd() const =0;

        /** returns the number of IBookmarks.
        */
        virtual sal_Int32 getBookmarksCount() const =0;

        /** Finds a bookmark by name.

            @param rName
            [in] the name of the bookmark to find.

            @returns
            an iterator pointing to the bookmark, or getBookmarksEnd() if nothing was found.
        */
        virtual const_iterator_t findBookmark(const OUString& rMark) const =0;

        /** Finds the first mark that is starting after.

            @returns
            an iterator pointing to the mark, or pointing to getBookmarksEnd() if nothing was found.
        */
        virtual const_iterator_t findFirstBookmarkStartsAfter(const SwPosition& rPos) const =0;

        /// Get the innermost bookmark that contains rPos.
        virtual sw::mark::IMark* getOneInnermostBookmarkFor(const SwPosition& rPos) const = 0;

        // Fieldmarks
        /** returns a STL-like random access iterator to the begin of the sequence of fieldmarks.
        */
        virtual const_iterator_t getFieldmarksBegin() const =0;

        /** returns a STL-like random access iterator to the end of the sequence of fieldmarks.
        */
        virtual const_iterator_t getFieldmarksEnd() const =0;

        /// returns the number of IFieldmarks.
        virtual sal_Int32 getFieldmarksCount() const = 0;

        /// get Fieldmark for CH_TXT_ATR_FIELDSTART/CH_TXT_ATR_FIELDEND at rPos
        virtual ::sw::mark::IFieldmark* getFieldmarkAt(const SwPosition& rPos) const =0;
        virtual ::sw::mark::IFieldmark* getFieldmarkFor(const SwPosition& pos) const =0;
        virtual sw::mark::IFieldmark* getFieldmarkBefore(const SwPosition& pos, bool bLoop) const =0;
        virtual sw::mark::IFieldmark* getFieldmarkAfter(const SwPosition& pos, bool bLoop) const =0;

        virtual ::sw::mark::IFieldmark* getDropDownFor(const SwPosition& pos) const=0;
        virtual std::vector<::sw::mark::IFieldmark*> getNoTextFieldmarksIn(const SwPaM &rPaM) const=0;

        virtual void deleteFieldmarkAt(const SwPosition& rPos) = 0;
        virtual ::sw::mark::IFieldmark* changeFormFieldmarkType(::sw::mark::IFieldmark* pFieldmark, const OUString& rNewType) = 0;

        virtual void NotifyCursorUpdate(const SwCursorShell& rCursorShell) = 0;
        virtual void ClearFieldActivation() = 0;

        // Annotation Marks
        virtual const_iterator_t getAnnotationMarksBegin() const = 0;
        virtual const_iterator_t getAnnotationMarksEnd() const = 0;
        virtual sal_Int32 getAnnotationMarksCount() const = 0;
        virtual const_iterator_t findAnnotationMark( const OUString& rName ) const = 0;
        virtual sw::mark::IMark* getAnnotationMarkFor(const SwPosition& rPosition) const = 0;
        // handle and restore text ranges of annotations of tracked deletions
        // based on the helper bookmarks (which can survive I/O and hiding redlines)
        virtual ::sw::mark::IMark* makeAnnotationBookmark(const SwPaM& rPaM,
            const OUString& rProposedName,
            MarkType eMark, ::sw::mark::InsertMode eMode,
            SwPosition const* pSepPos = nullptr) = 0;
        virtual const_iterator_t findAnnotationBookmark( const OUString& rName ) const = 0;
        virtual void restoreAnnotationMarks(bool bDelete = true) = 0;
        /** Finds the first mark that is starting after.

            @returns
            an iterator pointing to the mark, or pointing to getAnnotationMarksEnd() if nothing was found.
        */
        virtual const_iterator_t findFirstAnnotationStartsAfter(const SwPosition& rPos) const =0;

        /** Returns the MarkType used to create the mark
        */
        static SW_DLLPUBLIC MarkType GetType(const ::sw::mark::IMark& rMark);

        static SW_DLLPUBLIC OUString GetCrossRefHeadingBookmarkNamePrefix();
        static SW_DLLPUBLIC bool IsLegalPaMForCrossRefHeadingBookmark( const SwPaM& rPaM );
        static void DeleteFieldmarkCommand(::sw::mark::IFieldmark const& rMark);

    protected:
        virtual ~IDocumentMarkAccess() {};
};

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
