/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <sal/config.h>

#include "helper/qahelper.hxx"
#include "helper/debughelper.hxx"

#include <patattr.hxx>
#include <scitems.hxx>
#include <document.hxx>
#include <generalfunction.hxx>
#include <dpcache.hxx>
#include <dpobject.hxx>
#include <dpsave.hxx>
#include <dputil.hxx>
#include <attrib.hxx>
#include <dpshttab.hxx>
#include <globstr.hrc>
#include <scresid.hxx>
#include <queryentry.hxx>
#include <queryparam.hxx>
#include <rtl/string.hxx>

#include <test/xmltesttools.hxx>
#include <com/sun/star/sheet/DataPilotFieldOrientation.hpp>
#include <com/sun/star/sheet/DataPilotFieldGroupBy.hpp>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

class ScPivotTableFiltersTest : public ScModelTestBase
{
protected:
    virtual void registerNamespaces(xmlXPathContextPtr& pXmlXPathCtx) override;

public:
    ScPivotTableFiltersTest();

    // Import
    void testPivotTableBasicODS();
    void testPivotTableNamedRangeSourceODS();
    void testPivotTableSharedCacheGroupODS();
    void testGetPivotDataXLS();
    void testPivotTableSharedGroupXLSX();
    void testPivotTableSharedDateGroupXLSX(); // + export
    void testPivotTableSharedNestedDateGroupXLSX(); // + export
    void testPivotTableSharedNumGroupXLSX();
    void testPivotTableNoColumnsLayout();
    void testTdf112501();

    // Export
    void testPivotTableExportXLSX();
    void testPivotTableExportXLSXSingleDataField();
    void testPivotTableExportXLSXMultipleDataFields();
    void testPivotCacheExportXLSX();
    void testPivotTableXLSX();
    void testPivotTableTwoDataFieldsXLSX();
    void testPivotTableMedianODS();
    void testPivotTableRowHeaderXLS();
    void testPivotTableDoubleFieldFilter();
    void testPivotTableStringFieldFilter();
    void testPivotTableDateFieldFilter();
    void testPivotTableBoolFieldFilter();
    void testPivotTableRowColPageFieldFilter();
    void testPivotTableEmptyItem();
    void testPivotTablePageFieldFilter();
    void testPivotTableFirstHeaderRowXLSX();
    void testPivotTableDoubleFieldFilterXLSX();
    void testPivotTableStringFieldFilterXLSX();
    void testPivotTableDateFieldFilterXLSX();
    void testPivotTableBoolFieldFilterXLSX();
    void testPivotTableRowColPageFieldFilterXLSX();
    void testPivotTableErrorItemFilterXLSX();
    void testPivotTableErrorItemFilterXLSB();
    void testPivotTableErrorItem2FilterXLSX();
    void testPivotTableOutlineModeXLSX();
    void testPivotTableDuplicatedMemberFilterXLSX();
    void testPivotTableTabularModeXLSX();
    void testPivotTableDuplicateFields();
    void testTdf112106();
    void testTdf123923();
    void testTdf123939();
    void testTdf124651();
    void testTdf124736();
    void tesTtdf124772NumFmt();
    void testTdf124810();
    void testTdf124883();
    void testTdf125046();
    void testTdf125055();
    void testTdf125086();
    void testTdf73845();

    CPPUNIT_TEST_SUITE(ScPivotTableFiltersTest);

    CPPUNIT_TEST(testPivotTableBasicODS);
    CPPUNIT_TEST(testPivotTableNamedRangeSourceODS);
    CPPUNIT_TEST(testPivotTableSharedCacheGroupODS);
    CPPUNIT_TEST(testGetPivotDataXLS);
    CPPUNIT_TEST(testPivotTableSharedGroupXLSX);
    CPPUNIT_TEST(testPivotTableSharedDateGroupXLSX);
    CPPUNIT_TEST(testPivotTableSharedNestedDateGroupXLSX);
    CPPUNIT_TEST(testPivotTableSharedNumGroupXLSX);
    CPPUNIT_TEST(testPivotTableNoColumnsLayout);
    CPPUNIT_TEST(testTdf112501);

    CPPUNIT_TEST(testPivotTableExportXLSX);
    CPPUNIT_TEST(testPivotTableExportXLSXSingleDataField);
    CPPUNIT_TEST(testPivotTableExportXLSXMultipleDataFields);
    CPPUNIT_TEST(testPivotCacheExportXLSX);
    CPPUNIT_TEST(testPivotTableXLSX);
    CPPUNIT_TEST(testPivotTableTwoDataFieldsXLSX);
    CPPUNIT_TEST(testPivotTableMedianODS);
    CPPUNIT_TEST(testPivotTableRowHeaderXLS);
    CPPUNIT_TEST(testPivotTableDoubleFieldFilter);
    CPPUNIT_TEST(testPivotTableStringFieldFilter);
    CPPUNIT_TEST(testPivotTableDateFieldFilter);
    CPPUNIT_TEST(testPivotTableBoolFieldFilter);
    CPPUNIT_TEST(testPivotTableRowColPageFieldFilter);
    CPPUNIT_TEST(testPivotTableEmptyItem);
    CPPUNIT_TEST(testPivotTablePageFieldFilter);
    CPPUNIT_TEST(testPivotTableFirstHeaderRowXLSX);
    CPPUNIT_TEST(testPivotTableDoubleFieldFilterXLSX);
    CPPUNIT_TEST(testPivotTableStringFieldFilterXLSX);
    CPPUNIT_TEST(testPivotTableDateFieldFilterXLSX);
    CPPUNIT_TEST(testPivotTableBoolFieldFilterXLSX);
    CPPUNIT_TEST(testPivotTableRowColPageFieldFilterXLSX);
    CPPUNIT_TEST(testPivotTableErrorItemFilterXLSX);
    CPPUNIT_TEST(testPivotTableErrorItemFilterXLSB);
    CPPUNIT_TEST(testPivotTableErrorItem2FilterXLSX);
    CPPUNIT_TEST(testPivotTableOutlineModeXLSX);
    CPPUNIT_TEST(testPivotTableDuplicatedMemberFilterXLSX);
    CPPUNIT_TEST(testPivotTableTabularModeXLSX);
    CPPUNIT_TEST(testPivotTableDuplicateFields);
    CPPUNIT_TEST(testTdf112106);
    CPPUNIT_TEST(testTdf123923);
    CPPUNIT_TEST(testTdf123939);
    CPPUNIT_TEST(testTdf124651);
    CPPUNIT_TEST(testTdf124736);
    CPPUNIT_TEST(tesTtdf124772NumFmt);
    CPPUNIT_TEST(testTdf124810);
    CPPUNIT_TEST(testTdf124883);
    CPPUNIT_TEST(testTdf125046);
    CPPUNIT_TEST(testTdf125055);
    CPPUNIT_TEST(testTdf125086);
    CPPUNIT_TEST(testTdf73845);

    CPPUNIT_TEST_SUITE_END();
};

ScPivotTableFiltersTest::ScPivotTableFiltersTest()
    : ScModelTestBase("sc/qa/unit/data")
{
}

void ScPivotTableFiltersTest::registerNamespaces(xmlXPathContextPtr& pXmlXPathCtx)
{
    XmlTestTools::registerOOXMLNamespaces(pXmlXPathCtx);
}

namespace
{
class FindDimByName
{
    OUString maName;

public:
    explicit FindDimByName(const OUString& rName)
        : maName(rName)
    {
    }

    bool operator()(const ScDPSaveDimension* p) const { return p && p->GetName() == maName; }
};

bool hasDimension(const std::vector<const ScDPSaveDimension*>& rDims, const OUString& aName)
{
    return std::any_of(rDims.begin(), rDims.end(), FindDimByName(aName));
}
}

void ScPivotTableFiltersTest::testPivotTableBasicODS()
{
    createScDoc("ods/pivot-table-basic.ods");

    ScDocument* pDoc = getScDoc();
    CPPUNIT_ASSERT_EQUAL_MESSAGE("There should be exactly two sheets.", sal_Int16(2),
                                 pDoc->GetTableCount());

    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_MESSAGE("Failed to get a live ScDPCollection instance.", pDPs);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("There should be exactly one pivot table instance.", size_t(1),
                                 pDPs->GetCount());

    const ScDPObject* pDPObj = &(*pDPs)[0];
    CPPUNIT_ASSERT_MESSAGE("Failed to get a pivot table object.", pDPObj);
    const ScDPSaveData* pSaveData = pDPObj->GetSaveData();
    CPPUNIT_ASSERT_MESSAGE("Failed to get ScDPSaveData instance.", pSaveData);
    std::vector<const ScDPSaveDimension*> aDims;

    // Row fields
    pSaveData->GetAllDimensionsByOrientation(sheet::DataPilotFieldOrientation_ROW, aDims);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(("There should be exactly 3 row fields (2 normal dimensions and 1"
                                  " layout dimension)."),
                                 std::vector<ScDPSaveDimension const*>::size_type(3), aDims.size());
    CPPUNIT_ASSERT_MESSAGE("Dimension expected, but not found.", hasDimension(aDims, "Row1"));
    CPPUNIT_ASSERT_MESSAGE("Dimension expected, but not found.", hasDimension(aDims, "Row2"));
    const ScDPSaveDimension* pDataLayout = pSaveData->GetExistingDataLayoutDimension();
    CPPUNIT_ASSERT_MESSAGE("There should be a data layout field as a row field.", pDataLayout);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("There should be a data layout field as a row field.",
                                 sheet::DataPilotFieldOrientation_ROW,
                                 pDataLayout->GetOrientation());

    // Column fields
    pSaveData->GetAllDimensionsByOrientation(sheet::DataPilotFieldOrientation_COLUMN, aDims);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("There should be exactly 2 column fields.",
                                 std::vector<ScDPSaveDimension const*>::size_type(2), aDims.size());
    CPPUNIT_ASSERT_MESSAGE("Dimension expected, but not found.", hasDimension(aDims, "Col1"));
    CPPUNIT_ASSERT_MESSAGE("Dimension expected, but not found.", hasDimension(aDims, "Col2"));

    // Page fields
    pSaveData->GetAllDimensionsByOrientation(sheet::DataPilotFieldOrientation_PAGE, aDims);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("There should be exactly 2 page fields.",
                                 std::vector<ScDPSaveDimension const*>::size_type(2), aDims.size());
    CPPUNIT_ASSERT_MESSAGE("Dimension expected, but not found.", hasDimension(aDims, "Page1"));
    CPPUNIT_ASSERT_MESSAGE("Dimension expected, but not found.", hasDimension(aDims, "Page2"));

    // Check the data field.
    pSaveData->GetAllDimensionsByOrientation(sheet::DataPilotFieldOrientation_DATA, aDims);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("There should be exactly 1 data field.",
                                 std::vector<ScDPSaveDimension const*>::size_type(1), aDims.size());
    const ScDPSaveDimension* pDim = aDims.back();
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Function for the data field should be COUNT.",
                                 sal_uInt16(ScGeneralFunction::COUNT),
                                 sal_uInt16(pDim->GetFunction()));
}

void ScPivotTableFiltersTest::testPivotTableNamedRangeSourceODS()
{
    createScDoc("ods/pivot-table-named-range-source.ods");

    ScDocument* pDoc = getScDoc();

    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(size_t(1), pDPs->GetCount());

    ScDPObject* pDP = &(*pDPs)[0];
    CPPUNIT_ASSERT(pDP);

    // Make sure this pivot table is based on a named range source.
    const ScSheetSourceDesc* pDesc = pDP->GetSheetDesc();
    CPPUNIT_ASSERT(pDesc);
    CPPUNIT_ASSERT_EQUAL(OUString("MyRange"), pDesc->GetRangeName());

    css::sheet::DataPilotFieldOrientation nOrient;
    tools::Long nDim = pDP->GetHeaderDim(ScAddress(0, 1, 1), nOrient);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to detect header dimension.", tools::Long(0), nDim);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("This dimension should be a page dimension.",
                                 sheet::DataPilotFieldOrientation_PAGE, nOrient);
}

namespace
{
bool checkVisiblePageFieldMember(const ScDPSaveDimension::MemberList& rMembers,
                                 std::u16string_view rVisibleMember)
{
    bool bFound = false;
    for (const ScDPSaveMember* pMem : rMembers)
    {
        if (pMem->GetName() == rVisibleMember)
        {
            bFound = true;
            if (!pMem->GetIsVisible())
                // This member is supposed to be visible.  Fail.
                return false;
        }
        else
        {
            if (pMem->GetIsVisible())
                // This member is supposed to be hidden. Not good.
                return false;
        }
    }

    return bFound;
}
}

void ScPivotTableFiltersTest::testPivotTableSharedCacheGroupODS()
{
    createScDoc("ods/pivot-table-shared-cache-with-group.ods");
    ScDocument* pDoc = getScDoc();

    // Make sure that page field's visibility settings are loaded correctly.

    ScDPObject* pDPObj = pDoc->GetDPAtCursor(0, 0, 1); // A1 on 2nd sheet
    CPPUNIT_ASSERT_MESSAGE("There should be a pivot table here.", pDPObj);
    ScDPSaveData* pSaveData = pDPObj->GetSaveData();
    CPPUNIT_ASSERT_MESSAGE("Save data is expected.", pSaveData);
    ScDPSaveDimension* pDim = pSaveData->GetExistingDimensionByName(u"Project Name");
    CPPUNIT_ASSERT_MESSAGE("Failed to get page field named 'Project Name'.", pDim);
    const ScDPSaveDimension::MemberList* pMembers = &pDim->GetMembers();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), pMembers->size());
    CPPUNIT_ASSERT_MESSAGE("Incorrect member visibility.",
                           checkVisiblePageFieldMember(*pMembers, u"APL-01-1"));

    pDPObj = pDoc->GetDPAtCursor(0, 1, 2); // A2 on 3rd sheet
    CPPUNIT_ASSERT_MESSAGE("There should be a pivot table here.", pDPObj);
    pSaveData = pDPObj->GetSaveData();
    CPPUNIT_ASSERT_MESSAGE("Save data is expected.", pSaveData);
    pDim = pSaveData->GetExistingDimensionByName(u"Project Name");
    CPPUNIT_ASSERT_MESSAGE("Failed to get page field named 'Project Name'.", pDim);
    pMembers = &pDim->GetMembers();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), pMembers->size());
    CPPUNIT_ASSERT_MESSAGE("Incorrect member visibility.",
                           checkVisiblePageFieldMember(*pMembers, u"VEN-01-1"));

    // These two pivot tables share the same data range. We should only have
    // one pivot cache.
    ScDPCollection* pDPs = pDoc->GetDPCollection();
    ScDPCollection::SheetCaches& rSheetCaches = pDPs->GetSheetCaches();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rSheetCaches.size());

    // Make sure that the cache contains all group field data upon load.
    const ScSheetSourceDesc* pDesc = pDPObj->GetSheetDesc();
    CPPUNIT_ASSERT_MESSAGE("Failed to get the pivot source description instance.", pDesc);
    const ScDPCache* pCache = rSheetCaches.getExistingCache(pDesc->GetSourceRange());
    CPPUNIT_ASSERT_MESSAGE("Pivot cache should exist for this range.", pCache);

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), pCache->GetFieldCount());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), pCache->GetGroupFieldCount());

    SCCOL nDim = pCache->GetDimensionIndex(u"StartDate");
    CPPUNIT_ASSERT_MESSAGE("Dimension 'StartDate' doesn't exist in the cache.", nDim >= 0);
    sal_Int32 nGrpType = pCache->GetGroupType(nDim);
    CPPUNIT_ASSERT_EQUAL(sheet::DataPilotFieldGroupBy::DAYS, nGrpType);
    const ScDPNumGroupInfo* pInfo = pCache->GetNumGroupInfo(nDim);
    CPPUNIT_ASSERT_MESSAGE("Number group info doesn't exist in cache for 'StartDate'.", pInfo);

    // We should have two additional group fields and one should be years and
    // the other should be month.  The order is not guaranteed.

    bool bHasYears = false;
    bool bHasMonths = false;
    std::vector<SCROW> aMemberIds;

    for (tools::Long nGrpDim = 9; nGrpDim <= 10; ++nGrpDim)
    {
        nGrpType = pCache->GetGroupType(nGrpDim);
        switch (nGrpType)
        {
            case sheet::DataPilotFieldGroupBy::MONTHS:
            {
                bHasMonths = true;
                aMemberIds.clear();
                pCache->GetGroupDimMemberIds(nGrpDim, aMemberIds);

                // There should be a total of 14 items for the month group: 12
                // months plus the start and end value items.

                CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), aMemberIds.size());

                std::vector<sal_Int32> aGrpValues;
                for (size_t i = 0, n = aMemberIds.size(); i < n; ++i)
                {
                    const ScDPItemData* pItem = pCache->GetItemDataById(nGrpDim, aMemberIds[i]);
                    CPPUNIT_ASSERT_MESSAGE("Failed to get pivot item.", pItem);
                    CPPUNIT_ASSERT_EQUAL(ScDPItemData::GroupValue, pItem->GetType());
                    ScDPItemData::GroupValueAttr aGrpVal = pItem->GetGroupValue();
                    CPPUNIT_ASSERT_EQUAL(sheet::DataPilotFieldGroupBy::MONTHS, aGrpVal.mnGroupType);
                    aGrpValues.push_back(aGrpVal.mnValue);
                }

                std::sort(aGrpValues.begin(), aGrpValues.end());
                std::vector<sal_Int32> aChecks;
                aChecks.push_back(ScDPItemData::DateFirst);
                for (sal_Int32 i = 1; i <= 12; ++i)
                    aChecks.push_back(i); // January through December.
                aChecks.push_back(ScDPItemData::DateLast);
                CPPUNIT_ASSERT_MESSAGE("Unexpected group values for the month group.",
                                       bool(aGrpValues == aChecks));
            }
            break;
            case sheet::DataPilotFieldGroupBy::YEARS:
            {
                bHasYears = true;
                aMemberIds.clear();
                pCache->GetGroupDimMemberIds(nGrpDim, aMemberIds);

                // There should be a total of 4 items and they should be 2012,
                // 2013 and the start and end value items.

                CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), aMemberIds.size());

                std::vector<sal_Int32> aGrpValues;
                for (size_t i = 0, n = aMemberIds.size(); i < n; ++i)
                {
                    const ScDPItemData* pItem = pCache->GetItemDataById(nGrpDim, aMemberIds[i]);
                    CPPUNIT_ASSERT_MESSAGE("Failed to get pivot item.", pItem);
                    CPPUNIT_ASSERT_EQUAL(ScDPItemData::GroupValue, pItem->GetType());
                    ScDPItemData::GroupValueAttr aGrpVal = pItem->GetGroupValue();
                    CPPUNIT_ASSERT_EQUAL(sheet::DataPilotFieldGroupBy::YEARS, aGrpVal.mnGroupType);
                    aGrpValues.push_back(aGrpVal.mnValue);
                }

                std::sort(aGrpValues.begin(), aGrpValues.end());
                std::vector<sal_Int32> aChecks{ ScDPItemData::DateFirst, 2012, 2013,
                                                ScDPItemData::DateLast };
                CPPUNIT_ASSERT_MESSAGE("Unexpected group values for the year group.",
                                       bool(aGrpValues == aChecks));
            }
            break;
            default:;
        }
    }

    CPPUNIT_ASSERT_MESSAGE("Pivot cache doesn't have an additional year group.", bHasYears);
    CPPUNIT_ASSERT_MESSAGE("Pivot cache doesn't have an additional month group.", bHasMonths);
}

void ScPivotTableFiltersTest::testGetPivotDataXLS()
{
    createScDoc("xls/pivot-getpivotdata.xls");
    ScDocument* pDoc = getScDoc();
    pDoc->CalcAll();

    // Check GETPIVOTDATA results in E3:E20. Expected results are given in
    // F3:F20.

    for (SCROW nRow = 2; nRow <= 19; ++nRow)
        CPPUNIT_ASSERT_EQUAL(pDoc->GetValue(ScAddress(4, nRow, 1)),
                             pDoc->GetValue(ScAddress(5, nRow, 1)));
}

void ScPivotTableFiltersTest::testPivotTableSharedGroupXLSX()
{
    createScDoc("xlsx/pivot-table/shared-group-field.xlsx");
    ScDocument* pDoc = getScDoc();

    // Check whether right group names are imported for both tables
    // First table
    CPPUNIT_ASSERT_EQUAL(OUString("a2"), pDoc->GetString(ScAddress(0, 1, 0)));
    CPPUNIT_ASSERT_EQUAL(OUString("Csoport1"), pDoc->GetString(ScAddress(0, 2, 0)));
    CPPUNIT_ASSERT_EQUAL(OUString("Csoport2"), pDoc->GetString(ScAddress(0, 3, 0)));
    CPPUNIT_ASSERT_EQUAL(OUString("Csoport3"), pDoc->GetString(ScAddress(0, 4, 0)));
    CPPUNIT_ASSERT_EQUAL(OUString("16"), pDoc->GetString(ScAddress(0, 5, 0)));
    CPPUNIT_ASSERT_EQUAL(OUString("17"), pDoc->GetString(ScAddress(0, 6, 0)));
    CPPUNIT_ASSERT_EQUAL(OUString("18"), pDoc->GetString(ScAddress(0, 7, 0)));

    // Second table
    CPPUNIT_ASSERT_EQUAL(OUString("a2"), pDoc->GetString(ScAddress(0, 11, 0)));
    CPPUNIT_ASSERT_EQUAL(OUString("Csoport1"), pDoc->GetString(ScAddress(0, 12, 0)));
    CPPUNIT_ASSERT_EQUAL(OUString("Csoport2"), pDoc->GetString(ScAddress(0, 13, 0)));
    CPPUNIT_ASSERT_EQUAL(OUString("Csoport3"), pDoc->GetString(ScAddress(0, 14, 0)));
    CPPUNIT_ASSERT_EQUAL(OUString("16"), pDoc->GetString(ScAddress(0, 15, 0)));
    CPPUNIT_ASSERT_EQUAL(OUString("17"), pDoc->GetString(ScAddress(0, 16, 0)));
    CPPUNIT_ASSERT_EQUAL(OUString("18"), pDoc->GetString(ScAddress(0, 17, 0)));

    // There should be exactly 2 pivot tables and 1 cache.
    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), pDPs->GetCount());

    ScDPCollection::SheetCaches& rSheetCaches = pDPs->GetSheetCaches();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rSheetCaches.size());

    const ScDPCache* pCache = rSheetCaches.getExistingCache(ScRange(0, 0, 1, 13, 18, 1));
    CPPUNIT_ASSERT_MESSAGE("Pivot cache is expected for A1:N19 on the second sheet.", pCache);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), pCache->GetFieldCount());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pCache->GetGroupFieldCount());
}

void ScPivotTableFiltersTest::testPivotTableSharedDateGroupXLSX()
{
    auto testThis = [](ScDocument& rDoc) {
        // Check whether right date labels are imported for both tables
        // First table
        CPPUNIT_ASSERT_EQUAL(OUString("a"), rDoc.GetString(ScAddress(0, 3, 1)));
        CPPUNIT_ASSERT_EQUAL(OUString("1965"), rDoc.GetString(ScAddress(0, 4, 1)));
        CPPUNIT_ASSERT_EQUAL(OUString("1989"), rDoc.GetString(ScAddress(0, 5, 1)));
        CPPUNIT_ASSERT_EQUAL(OUString("2000"), rDoc.GetString(ScAddress(0, 6, 1)));
        CPPUNIT_ASSERT_EQUAL(OUString("2004"), rDoc.GetString(ScAddress(0, 7, 1)));
        // TODO: check why this fails with 2005
        // CPPUNIT_ASSERT_EQUAL(OUString("2007"), rDoc.GetString(ScAddress(0,8,1)));

        // Second table
        CPPUNIT_ASSERT_EQUAL(OUString("a"), rDoc.GetString(ScAddress(5, 3, 1)));
        CPPUNIT_ASSERT_EQUAL(OUString("1965"), rDoc.GetString(ScAddress(5, 4, 1)));
        CPPUNIT_ASSERT_EQUAL(OUString("1989"), rDoc.GetString(ScAddress(5, 5, 1)));
        CPPUNIT_ASSERT_EQUAL(OUString("2000"), rDoc.GetString(ScAddress(5, 6, 1)));
        CPPUNIT_ASSERT_EQUAL(OUString("2004"), rDoc.GetString(ScAddress(5, 7, 1)));
        // TODO: check why this fails with 2005
        // CPPUNIT_ASSERT_EQUAL(OUString("2007"), rDoc.GetString(ScAddress(5,8,1)));

        // There should be exactly 2 pivot tables and 1 cache.
        ScDPCollection* pDPs = rDoc.GetDPCollection();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), pDPs->GetCount());

        ScDPCollection::SheetCaches& rSheetCaches = pDPs->GetSheetCaches();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rSheetCaches.size());

        const ScDPCache* pCache = rSheetCaches.getExistingCache(ScRange(0, 0, 0, 9, 24, 0));
        CPPUNIT_ASSERT_MESSAGE("Pivot cache is expected for A1:J25 on the first sheet.", pCache);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), pCache->GetFieldCount());
    };
    createScDoc("xlsx/pivot-table/shared-dategroup.xlsx");
    testThis(*getScDoc());
    // Now test round-trip of group fields
    saveAndReload("Calc Office Open XML");
    testThis(*getScDoc());
}

void ScPivotTableFiltersTest::testPivotTableSharedNestedDateGroupXLSX()
{
    auto testThis = [](ScDocument& rDoc) {
        // Check whether right date groups are imported for both tables
        // First table
        CPPUNIT_ASSERT_EQUAL(OUString("Years"), rDoc.GetString(ScAddress(0, 3, 1)));
        CPPUNIT_ASSERT_EQUAL(OUString("1965"), rDoc.GetString(ScAddress(0, 4, 1)));
        CPPUNIT_ASSERT_EQUAL(OUString("1989"), rDoc.GetString(ScAddress(0, 11, 1)));
        CPPUNIT_ASSERT_EQUAL(OUString("2000"), rDoc.GetString(ScAddress(0, 18, 1)));
        CPPUNIT_ASSERT_EQUAL(OUString("2004"), rDoc.GetString(ScAddress(0, 21, 1)));
        // TODO: check why this fails with the empty string
        //CPPUNIT_ASSERT_EQUAL(OUString("2007"), rDoc.GetString(ScAddress(0,32,1)));
        CPPUNIT_ASSERT_EQUAL(OUString("Quarters"), rDoc.GetString(ScAddress(1, 3, 1)));
        CPPUNIT_ASSERT_EQUAL(OUString("a"), rDoc.GetString(ScAddress(2, 3, 1)));

        // Second table
        CPPUNIT_ASSERT_EQUAL(OUString("Years"), rDoc.GetString(ScAddress(6, 3, 1)));
        CPPUNIT_ASSERT_EQUAL(OUString("1965"), rDoc.GetString(ScAddress(6, 4, 1)));
        CPPUNIT_ASSERT_EQUAL(OUString("1989"), rDoc.GetString(ScAddress(6, 11, 1)));
        CPPUNIT_ASSERT_EQUAL(OUString("2000"), rDoc.GetString(ScAddress(6, 18, 1)));
        CPPUNIT_ASSERT_EQUAL(OUString("2004"), rDoc.GetString(ScAddress(6, 21, 1)));
        // TODO: check why this fails with the empty string
        //CPPUNIT_ASSERT_EQUAL(OUString("2007"), rDoc.GetString(ScAddress(6,31,1)));
        CPPUNIT_ASSERT_EQUAL(OUString("Quarters"), rDoc.GetString(ScAddress(7, 3, 1)));
        CPPUNIT_ASSERT_EQUAL(OUString("a"), rDoc.GetString(ScAddress(8, 3, 1)));

        // There should be exactly 2 pivot tables and 1 cache.
        ScDPCollection* pDPs = rDoc.GetDPCollection();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), pDPs->GetCount());

        ScDPCollection::SheetCaches& rSheetCaches = pDPs->GetSheetCaches();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rSheetCaches.size());

        const ScDPCache* pCache = rSheetCaches.getExistingCache(ScRange(0, 0, 0, 9, 24, 0));
        CPPUNIT_ASSERT_MESSAGE("Pivot cache is expected for A1:J25 on the first sheet.", pCache);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), pCache->GetFieldCount());
        // Two new group field is created
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), pCache->GetGroupFieldCount());
    };

    createScDoc("xlsx/pivot-table/shared-nested-dategroup.xlsx");
    testThis(*getScDoc());
    // Now test round-trip of group fields
    saveAndReload("Calc Office Open XML");
    testThis(*getScDoc());
}

void ScPivotTableFiltersTest::testPivotTableSharedNumGroupXLSX()
{
    createScDoc("xlsx/pivot-table/shared-numgroup.xlsx");
    ScDocument* pDoc = getScDoc();

    // Check whether right number groups are imported for both tables
    // First table
    CPPUNIT_ASSERT_EQUAL(OUString("f"), pDoc->GetString(ScAddress(0, 3, 1)));
    CPPUNIT_ASSERT_EQUAL(OUString("32674-47673"), pDoc->GetString(ScAddress(0, 4, 1)));
    CPPUNIT_ASSERT_EQUAL(OUString("47674-62673"), pDoc->GetString(ScAddress(0, 5, 1)));
    CPPUNIT_ASSERT_EQUAL(OUString("62674-77673"), pDoc->GetString(ScAddress(0, 6, 1)));
    CPPUNIT_ASSERT_EQUAL(OUString("77674-92673"), pDoc->GetString(ScAddress(0, 7, 1)));
    CPPUNIT_ASSERT_EQUAL(OUString("92674-107673"), pDoc->GetString(ScAddress(0, 8, 1)));

    // Second table
    CPPUNIT_ASSERT_EQUAL(OUString("f"), pDoc->GetString(ScAddress(5, 3, 1)));
    CPPUNIT_ASSERT_EQUAL(OUString("32674-47673"), pDoc->GetString(ScAddress(5, 4, 1)));
    CPPUNIT_ASSERT_EQUAL(OUString("47674-62673"), pDoc->GetString(ScAddress(5, 5, 1)));
    CPPUNIT_ASSERT_EQUAL(OUString("62674-77673"), pDoc->GetString(ScAddress(5, 6, 1)));
    CPPUNIT_ASSERT_EQUAL(OUString("77674-92673"), pDoc->GetString(ScAddress(5, 7, 1)));
    CPPUNIT_ASSERT_EQUAL(OUString("92674-107673"), pDoc->GetString(ScAddress(5, 8, 1)));

    // There should be exactly 2 pivot tables and 1 cache.
    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), pDPs->GetCount());

    ScDPCollection::SheetCaches& rSheetCaches = pDPs->GetSheetCaches();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rSheetCaches.size());

    const ScDPCache* pCache = rSheetCaches.getExistingCache(ScRange(0, 0, 0, 9, 24, 0));
    CPPUNIT_ASSERT_MESSAGE("Pivot cache is expected for A1:J25 on the first sheet.", pCache);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), pCache->GetFieldCount());
}

void ScPivotTableFiltersTest::testPivotTableNoColumnsLayout()
{
    // tdf#113268 - Pivot table: Missing popup button after opening a pivot table from ODS
    createScDoc("ods/pivottable_no_columns_layout.ods");
    ScDocument* pDoc = getScDoc();

    // There should be exactly 2 pivot tables
    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), pDPs->GetCount());

    // Check first pivot table's popup button (headerlayout flag)
    {
        const ScPatternAttr* pPattern = pDoc->GetPattern(0, 1, 1);
        const SfxPoolItem& rPoolItem = pPattern->GetItem(ATTR_MERGE_FLAG);
        const ScMergeFlagAttr& rMergeFlag = static_cast<const ScMergeFlagAttr&>(rPoolItem);
        CPPUNIT_ASSERT(rMergeFlag.GetValue() & ScMF::ButtonPopup);
    }

    // Check second pivot table's popup button
    {
        const ScPatternAttr* pPattern = pDoc->GetPattern(3, 0, 1);
        const SfxPoolItem& rPoolItem = pPattern->GetItem(ATTR_MERGE_FLAG);
        const ScMergeFlagAttr& rMergeFlag = static_cast<const ScMergeFlagAttr&>(rPoolItem);
        CPPUNIT_ASSERT(rMergeFlag.GetValue() & ScMF::ButtonPopup);
    }
}

void ScPivotTableFiltersTest::testTdf112501()
{
    createScDoc("xls/tdf112501.xls");
    ScDocument* pDoc = getScDoc();

    // There should be exactly 2 pivot tables
    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), pDPs->GetCount());

    // Check first pivot table popup buttons (compact)
    {
        const ScDPObject* pDPObj = &(*pDPs)[0];
        CPPUNIT_ASSERT_MESSAGE("Failed to get a pivot table object.", pDPObj);
        // Check whether we have the buttons at the right buttons
        // Row button
        {
            const ScPatternAttr* pPattern = pDoc->GetPattern(0, 3, 0);
            const SfxPoolItem& rPoolItem = pPattern->GetItem(ATTR_MERGE_FLAG);
            const ScMergeFlagAttr& rMergeFlag = static_cast<const ScMergeFlagAttr&>(rPoolItem);
            CPPUNIT_ASSERT(rMergeFlag.GetValue() & ScMF::ButtonPopup);
        }
        // Column button
        {
            const ScPatternAttr* pPattern = pDoc->GetPattern(1, 2, 0);
            const SfxPoolItem& rPoolItem = pPattern->GetItem(ATTR_MERGE_FLAG);
            const ScMergeFlagAttr& rMergeFlag = static_cast<const ScMergeFlagAttr&>(rPoolItem);
            CPPUNIT_ASSERT(rMergeFlag.GetValue() & ScMF::ButtonPopup);
        }
        // Check also C3 to make sure column button is not placed there
        {
            const ScPatternAttr* pPattern = pDoc->GetPattern(2, 2, 0);
            const SfxPoolItem& rPoolItem = pPattern->GetItem(ATTR_MERGE_FLAG);
            const ScMergeFlagAttr& rMergeFlag = static_cast<const ScMergeFlagAttr&>(rPoolItem);
            CPPUNIT_ASSERT(!(rMergeFlag.GetValue() & ScMF::ButtonPopup));
        }
    }

    // Check first pivot table popup buttons (not compact)
    {
        const ScDPObject* pDPObj = &(*pDPs)[1];
        CPPUNIT_ASSERT_MESSAGE("Failed to get a pivot table object.", pDPObj);
        // Check whether we have the buttons at the right buttons
        // Two row buttons
        {
            const ScPatternAttr* pPattern = pDoc->GetPattern(7, 3, 0);
            const SfxPoolItem& rPoolItem = pPattern->GetItem(ATTR_MERGE_FLAG);
            const ScMergeFlagAttr& rMergeFlag = static_cast<const ScMergeFlagAttr&>(rPoolItem);
            CPPUNIT_ASSERT(rMergeFlag.GetValue() & ScMF::ButtonPopup);
        }
        {
            const ScPatternAttr* pPattern = pDoc->GetPattern(8, 3, 0);
            const SfxPoolItem& rPoolItem = pPattern->GetItem(ATTR_MERGE_FLAG);
            const ScMergeFlagAttr& rMergeFlag = static_cast<const ScMergeFlagAttr&>(rPoolItem);
            CPPUNIT_ASSERT(rMergeFlag.GetValue() & ScMF::ButtonPopup);
        }
        // Column button
        {
            const ScPatternAttr* pPattern = pDoc->GetPattern(9, 2, 0);
            const SfxPoolItem& rPoolItem = pPattern->GetItem(ATTR_MERGE_FLAG);
            const ScMergeFlagAttr& rMergeFlag = static_cast<const ScMergeFlagAttr&>(rPoolItem);
            CPPUNIT_ASSERT(rMergeFlag.GetValue() & ScMF::ButtonPopup);
        }
    }
}

void ScPivotTableFiltersTest::testPivotTableExportXLSX()
{
    // tdf#89139: pivot table definition needs to list items, including hidden

    createScDoc("xlsx/tdf89139_pivot_table.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pTable = parseExport("xl/pivotTables/pivotTable1.xml");
    CPPUNIT_ASSERT(pTable);

    assertXPath(pTable, "/x:pivotTableDefinition/x:pivotFields/x:pivotField[3]/x:items", "count",
                "4");
    assertXPath(pTable, "/x:pivotTableDefinition/x:pivotFields/x:pivotField[3]/x:items/x:item", 4);
    assertXPath(pTable, "/x:pivotTableDefinition/x:pivotFields/x:pivotField[3]/x:items/x:item[3]",
                "h", "1");
}

void ScPivotTableFiltersTest::testPivotTableExportXLSXSingleDataField()
{
    createScDoc("ods/tdf123421_1datafield.ods");

    save("Calc Office Open XML");
    xmlDocUniquePtr pTable = parseExport("xl/pivotTables/pivotTable1.xml");
    CPPUNIT_ASSERT(pTable);

    assertXPath(pTable, "/x:pivotTableDefinition/x:location", "ref", "A3:B6");
    assertXPath(pTable, "/x:pivotTableDefinition/x:location", "firstHeaderRow", "1");
    assertXPath(pTable, "/x:pivotTableDefinition/x:location", "firstDataRow", "1");
    assertXPath(pTable, "/x:pivotTableDefinition/x:location", "firstDataCol", "1");
    assertXPath(pTable, "/x:pivotTableDefinition/x:dataFields", "count", "1");

    // There should not be any colFields tag, before the fix there used to be a singleton with
    // <field x="-2"/> as child node.
    assertXPath(pTable, "/x:pivotTableDefinition/x:colFields", 0);
}

void ScPivotTableFiltersTest::testPivotTableExportXLSXMultipleDataFields()
{
    createScDoc("ods/tdf123421_2datafields.ods");

    save("Calc Office Open XML");
    xmlDocUniquePtr pTable = parseExport("xl/pivotTables/pivotTable1.xml");
    CPPUNIT_ASSERT(pTable);

    assertXPath(pTable, "/x:pivotTableDefinition/x:location", "ref", "A1:C6");
    assertXPath(pTable, "/x:pivotTableDefinition/x:location", "firstHeaderRow", "1");
    assertXPath(pTable, "/x:pivotTableDefinition/x:location", "firstDataRow", "2");
    assertXPath(pTable, "/x:pivotTableDefinition/x:location", "firstDataCol", "1");

    assertXPath(pTable, "/x:pivotTableDefinition/x:dataFields", "count", "2");

    // There should be a single colFields tag with sole child node
    // <field x="-2"/>.
    assertXPath(pTable, "/x:pivotTableDefinition/x:colFields", 1);
    assertXPath(pTable, "/x:pivotTableDefinition/x:colFields", "count", "1");
    assertXPath(pTable, "/x:pivotTableDefinition/x:colFields/x:field", 1);
    assertXPath(pTable, "/x:pivotTableDefinition/x:colFields/x:field", "x", "-2");
}

void ScPivotTableFiltersTest::testPivotCacheExportXLSX()
{
    // tdf#89139 FILESAVE xlsx pivot table corrupted after save with LO and re-open with MS Office
    // MS Excel is very sensitive for proper values of fields:
    // containsMixedTypes, containsSemiMixedTypes, containsInteger, containsBlank
    // If it is not properly set, then Excel is not opening spreadsheet properly.
    // This test case ensures, that such values are properly set according to documentation:
    // https://technet.microsoft.com/en-us/library/documentformat.openxml.spreadsheet.shareditems.aspx

    createScDoc("xlsx/pivot-table/with-strings-integers-and-dates.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pCacheDef = parseExport("xl/pivotCache/pivotCacheDefinition1.xml");
    CPPUNIT_ASSERT(pCacheDef);

    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField", 6);

    // Four strings and one empty field
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[1]", "name",
                "mixed strings and empty");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[1]/x:sharedItems",
                "containsBlank", "1");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[1]/x:sharedItems",
                           "containsMixedTypes");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[1]/x:sharedItems",
                           "containsSemiMixedTypes");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[1]/x:sharedItems",
                           "containsNonDate");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[1]/x:sharedItems",
                           "containsDate");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[1]/x:sharedItems",
                           "containsString");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[1]/x:sharedItems",
                           "minDate");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[1]/x:sharedItems",
                           "maxDate");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[1]/x:sharedItems",
                           "containsNumber");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[1]/x:sharedItems",
                           "containsInteger");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[1]/x:sharedItems",
                           "minValue");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[1]/x:sharedItems",
                           "maxValue");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[1]/x:sharedItems",
                "count", "5");

    // Two integers and one empty field
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[2]", "name",
                "mixed empty fields and integers");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[2]/x:sharedItems",
                "containsBlank", "1");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[2]/x:sharedItems",
                           "containsMixedTypes");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[2]/x:sharedItems",
                           "containsSemiMixedTypes");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[2]/x:sharedItems",
                           "containsNonDate");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[2]/x:sharedItems",
                           "containsDate");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[2]/x:sharedItems",
                "containsString", "0");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[2]/x:sharedItems",
                           "minDate");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[2]/x:sharedItems",
                           "maxDate");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[2]/x:sharedItems",
                "containsNumber", "1");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[2]/x:sharedItems",
                "containsInteger", "1");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[2]/x:sharedItems",
                "minValue", "111");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[2]/x:sharedItems",
                "maxValue", "222");
    // We list items on round-trip, Excel not, but Excel accepts that; We need list all items, because we are using it as reference in pivotCacheRecords1.xml
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[2]/x:sharedItems",
                "count", "3");

    // Five integers
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[3]", "name",
                "all fields are integers");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[3]/x:sharedItems",
                           "containsBlank");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[3]/x:sharedItems",
                           "containsMixedTypes");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[3]/x:sharedItems",
                "containsSemiMixedTypes", "0");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[3]/x:sharedItems",
                           "containsNonDate");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[3]/x:sharedItems",
                           "containsDate");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[3]/x:sharedItems",
                "containsString", "0");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[3]/x:sharedItems",
                           "minDate");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[3]/x:sharedItems",
                           "maxDate");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[3]/x:sharedItems",
                "containsNumber", "1");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[3]/x:sharedItems",
                "containsInteger", "1");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[3]/x:sharedItems",
                "minValue", "1111");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[3]/x:sharedItems",
                "maxValue", "5555");
    // We list items on round-trip, Excel not, but Excel accepts that; We need list all items, because we are using it as reference in pivotCacheRecords1.xml
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[3]/x:sharedItems",
                "count", "5");

    // Three integers and one string
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[4]", "name",
                "mixed strings and integers");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[4]/x:sharedItems",
                           "containsBlank");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[4]/x:sharedItems",
                "containsMixedTypes", "1");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[4]/x:sharedItems",
                           "containsSemiMixedTypes");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[4]/x:sharedItems",
                           "containsNonDate");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[4]/x:sharedItems",
                           "containsDate");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[4]/x:sharedItems",
                           "containsString");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[4]/x:sharedItems",
                           "minDate");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[4]/x:sharedItems",
                           "maxDate");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[4]/x:sharedItems",
                "containsNumber", "1");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[4]/x:sharedItems",
                "containsInteger", "1");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[4]/x:sharedItems",
                "minValue", "1234");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[4]/x:sharedItems",
                "maxValue", "5678");
    // We list items on round-trip, Excel not, but Excel accepts that; We need list all items, because we are using it as reference in pivotCacheRecords1.xml
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[4]/x:sharedItems",
                "count", "4");

    // Four dates without blanks
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[5]", "name",
                "date and time with duplicated entries");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[5]/x:sharedItems",
                           "containsBlank");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[5]/x:sharedItems",
                           "containsMixedTypes");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[5]/x:sharedItems",
                "containsSemiMixedTypes", "0");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[5]/x:sharedItems",
                "containsNonDate", "0");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[5]/x:sharedItems",
                "containsDate", "1");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[5]/x:sharedItems",
                "containsString", "0");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[5]/x:sharedItems",
                "minDate", "1899-12-31T00:00:00");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[5]/x:sharedItems",
                "maxDate", "2009-07-06T10:53:02");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[5]/x:sharedItems",
                           "containsNumber");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[5]/x:sharedItems",
                           "containsInteger");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[5]/x:sharedItems",
                           "minValue");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[5]/x:sharedItems",
                           "maxValue");
    // We list items on round-trip, Excel not, but Excel accepts that; We need list all items, because we are using it as reference in pivotCacheRecords1.xml
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[5]/x:sharedItems",
                "count", "4");

    // Only blanks
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[6]", "name",
                "blank");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[6]/x:sharedItems",
                "containsBlank", "1");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[6]/x:sharedItems",
                           "containsMixedTypes");
    // Despite what documentation says, in case there's only blank values in field (no strings), containsSemiMixedTypes is true (default - not written)
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[6]/x:sharedItems",
                           "containsSemiMixedTypes");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[6]/x:sharedItems",
                           "containsDate");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[6]/x:sharedItems",
                "containsString", "0");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[6]/x:sharedItems",
                           "minDate");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[6]/x:sharedItems",
                           "maxDate");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[6]/x:sharedItems",
                           "containsNumber");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[6]/x:sharedItems",
                           "containsInteger");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[6]/x:sharedItems",
                           "minValue");
    assertXPathNoAttribute(pCacheDef,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[6]/x:sharedItems",
                           "maxValue");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[6]/x:sharedItems",
                "count", "1");
}

void ScPivotTableFiltersTest::testPivotTableXLSX()
{
    struct
    {
        bool check(const ScDocument& rDoc)
        {
            if (!rDoc.HasPivotTable())
            {
                cerr << "The document should have pivot table." << endl;
                return false;
            }

            const ScDPCollection* pDPs = rDoc.GetDPCollection();
            if (!pDPs)
            {
                cerr << "Pivot table container should exist." << endl;
                return false;
            }

            ScRange aSrcRange(0, 0, 0, 9, 2, 0); // A1:J3 on Sheet1.
            const ScDPCache* pCache = pDPs->GetSheetCaches().getExistingCache(aSrcRange);
            if (!pCache)
            {
                cerr << "The document should have a pivot cache for A1:J3 on Sheet1." << endl;
                return false;
            }

            // Cache should have fields from F1 through F10.

            const char* pNames[] = { "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10" };

            size_t nCount = pCache->GetFieldCount();
            if (nCount != SAL_N_ELEMENTS(pNames))
            {
                cout << "Incorrect number of fields in pivot cache." << endl;
                return false;
            }

            for (size_t i = 0; i < nCount; ++i)
            {
                OUString aCacheName = pCache->GetDimensionName(i);
                if (aCacheName != OUString::createFromAscii(pNames[i]))
                {
                    cerr << "Field " << i << " has label '" << aCacheName << "' but expected '"
                         << pNames[i] << "'" << endl;
                    return false;
                }
            }

            const ScDPObject* pDPObj = rDoc.GetDPAtCursor(0, 10, 0); // A11
            if (!pDPObj)
            {
                cerr << "A pivot table should exist over A11." << endl;
                return false;
            }

            // Output range should be A8:D15.
            ScRange aOutRange = pDPObj->GetOutRange();
            if (ScRange(0, 7, 0, 3, 14, 0) != aOutRange)
            {
                cerr << "Incorrect output range." << endl;
                return false;
            }

            // Row field - F1
            // Column field - F4
            // Page fields - F7 and F6
            // Data field - F10

            const ScDPSaveData* pSaveData = pDPObj->GetSaveData();
            if (!pSaveData)
            {
                cerr << "Save data should exist in each pivot table object." << endl;
                return false;
            }

            std::vector<const ScDPSaveDimension*> aDims;
            pSaveData->GetAllDimensionsByOrientation(sheet::DataPilotFieldOrientation_ROW, aDims);
            if (aDims.size() != 1 || aDims[0]->GetName() != "F1")
            {
                cerr << "Pivot table should have one row field labelled 'F1'" << endl;
                return false;
            }

            pSaveData->GetAllDimensionsByOrientation(sheet::DataPilotFieldOrientation_COLUMN,
                                                     aDims);
            if (aDims.size() != 1 || aDims[0]->GetName() != "F4")
            {
                cerr << "Pivot table should have one column field labelled 'F4'" << endl;
                return false;
            }

            pSaveData->GetAllDimensionsByOrientation(sheet::DataPilotFieldOrientation_PAGE, aDims);
            if (aDims.size() != 2 || aDims[0]->GetName() != "F7" || aDims[1]->GetName() != "F6")
            {
                cerr << "Pivot table should have two page fields labelled 'F7' and 'F6' in this "
                        "order."
                     << endl;
                return false;
            }

            pSaveData->GetAllDimensionsByOrientation(sheet::DataPilotFieldOrientation_DATA, aDims);
            if (aDims.size() != 1 || aDims[0]->GetName() != "F10")
            {
                cerr << "Pivot table should have one data field labelled 'F10'" << endl;
                return false;
            }

            const ScDPSaveDimension* pDim = aDims[0];
            if (pDim->GetFunction() != ScGeneralFunction::SUM)
            {
                cerr << "Data field should have SUM function." << endl;
                return false;
            }

            return true;
        }

    } aTest;

    createScDoc("xlsx/pivot-table/many-fields-in-cache.xlsx");
    ScDocument* pDoc = getScDoc();

    // Initial check.
    bool bCheck = aTest.check(*pDoc);
    CPPUNIT_ASSERT_MESSAGE("Initial check failed.", bCheck);

    saveAndReload("Calc Office Open XML");
    pDoc = getScDoc();

    // Reload check.
    bCheck = aTest.check(*pDoc);
    CPPUNIT_ASSERT_MESSAGE("Reload check failed.", bCheck);
}

void ScPivotTableFiltersTest::testPivotTableTwoDataFieldsXLSX()
{
    struct
    {
        bool check(const ScDocument& rDoc)
        {
            if (!rDoc.HasPivotTable())
            {
                cerr << "The document should have pivot table." << endl;
                return false;
            }

            const ScDPCollection* pDPs = rDoc.GetDPCollection();
            if (!pDPs)
            {
                cerr << "Pivot table container should exist." << endl;
                return false;
            }

            ScRange aSrcRange(1, 1, 1, 2, 8, 1); // B2:C9 on the 2nd sheet.
            const ScDPCache* pCache = pDPs->GetSheetCaches().getExistingCache(aSrcRange);
            if (!pCache)
            {
                cerr << "The document should have a pivot cache for B2:C9 on 'Src'." << endl;
                return false;
            }

            const char* pNames[] = { "Name", "Value" };
            (void)pNames;

            size_t nCount = pCache->GetFieldCount();
            if (nCount != SAL_N_ELEMENTS(pNames))
            {
                cout << "Incorrect number of fields in pivot cache." << endl;
                return false;
            }

            const ScDPObject* pDPObj = rDoc.GetDPAtCursor(0, 2, 0); // A3
            if (!pDPObj)
            {
                cerr << "A pivot table should exist over A3." << endl;
                return false;
            }

            // Output range should be A3:C12.
            ScRange aOutRange = pDPObj->GetOutRange();
            if (ScRange(0, 2, 0, 2, 11, 0) != aOutRange)
            {
                cerr << "Incorrect output range." << endl;
                return false;
            }

            const ScDPSaveData* pSaveData = pDPObj->GetSaveData();
            if (!pSaveData)
            {
                cerr << "Save data should exist in each pivot table object." << endl;
                return false;
            }

            std::vector<const ScDPSaveDimension*> aDims;
            pSaveData->GetAllDimensionsByOrientation(sheet::DataPilotFieldOrientation_ROW, aDims);
            if (aDims.size() != 1 || aDims[0]->GetName() != "Name")
            {
                cerr << "Pivot table should have one row field labelled 'Name'" << endl;
                return false;
            }

            pSaveData->GetAllDimensionsByOrientation(sheet::DataPilotFieldOrientation_DATA, aDims);
            if (aDims.size() != 2
                || ScDPUtil::getSourceDimensionName(aDims[0]->GetName()) != "Value"
                || ScDPUtil::getSourceDimensionName(aDims[1]->GetName()) != "Value")
            {
                cerr << "Pivot table should have two duplicated data fields both of which are "
                        "named 'Value'."
                     << endl;
                return false;
            }

            if (aDims[0]->GetFunction() != ScGeneralFunction::SUM)
            {
                cerr << "First data field should be SUM." << endl;
                return false;
            }

            if (aDims[1]->GetFunction() != ScGeneralFunction::COUNT)
            {
                cerr << "First data field should be COUNT." << endl;
                return false;
            }

            pSaveData->GetAllDimensionsByOrientation(sheet::DataPilotFieldOrientation_COLUMN,
                                                     aDims);
            if (aDims.size() != 1 || !aDims[0]->IsDataLayout())
            {
                cerr << "Pivot table should have one column field which is a data layout field."
                     << endl;
                return false;
            }

            return true;
        }

    } aTest;

    createScDoc("xlsx/pivot-table/two-data-fields.xlsx");
    ScDocument* pDoc = getScDoc();

    // Initial check.
    bool bCheck = aTest.check(*pDoc);
    CPPUNIT_ASSERT_MESSAGE("Initial check failed.", bCheck);

    saveAndReload("Calc Office Open XML");
    pDoc = getScDoc();

    // Reload check.
    bCheck = aTest.check(*pDoc);
    CPPUNIT_ASSERT_MESSAGE("Reload check failed.", bCheck);
}

void ScPivotTableFiltersTest::testPivotTableMedianODS()
{
    createScDoc("ods/pivot-table-median.ods");

    // Export the document and import again for a check
    saveAndReload("calc8");

    // Check sheet
    ScDocument* pDoc = getScDoc();
    CPPUNIT_ASSERT_EQUAL_MESSAGE("There should be exactly one sheet.", sal_Int16(1),
                                 pDoc->GetTableCount());

    // Check pivot table
    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_MESSAGE("Failed to get a live ScDPCollection instance.", pDPs);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("There should be one pivot table instance.", size_t(1),
                                 pDPs->GetCount());
    const ScDPObject* pDPObj = &(*pDPs)[0];
    CPPUNIT_ASSERT_MESSAGE("Failed to get pivot table object.", pDPObj);
    const ScDPSaveData* pSaveData = pDPObj->GetSaveData();
    CPPUNIT_ASSERT_MESSAGE("Failed to get ScDPSaveData instance.", pSaveData);

    // Check the data field function.
    std::vector<const ScDPSaveDimension*> aDims;
    pSaveData->GetAllDimensionsByOrientation(sheet::DataPilotFieldOrientation_DATA, aDims);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("There should be exactly 1 data field.",
                                 std::vector<ScDPSaveDimension const*>::size_type(1), aDims.size());

    const ScDPSaveDimension* pDim = aDims.back();
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Function for the data field should be MEDIAN.",
                                 sal_uInt16(ScGeneralFunction::MEDIAN),
                                 sal_uInt16(pDim->GetFunction()));
}

void ScPivotTableFiltersTest::testPivotTableRowHeaderXLS()
{
    createScDoc("xls/pivot_row_header.xls");
    ScDocument* pDoc = getScDoc();

    // There should be exactly 2 pivot tables
    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), pDPs->GetCount());

    // First table has extra row header
    {
        const ScDPObject* pDPObj = &(*pDPs)[0];
        CPPUNIT_ASSERT_MESSAGE("Failed to get a pivot table object.", pDPObj);
        CPPUNIT_ASSERT(pDPObj->GetHeaderLayout());
        // Check whether the row header has the right popupbutton flag
        const ScPatternAttr* pPattern = pDoc->GetPattern(3, 3, 0);
        const SfxPoolItem& rPoolItem = pPattern->GetItem(ATTR_MERGE_FLAG);
        const ScMergeFlagAttr& rMergeFlag = static_cast<const ScMergeFlagAttr&>(rPoolItem);
        CPPUNIT_ASSERT(rMergeFlag.GetValue() & ScMF::ButtonPopup);
    }

    // Second table has no extra row header
    {
        const ScDPObject* pDPObj = &(*pDPs)[1];
        CPPUNIT_ASSERT_MESSAGE("Failed to get a pivot table object.", pDPObj);
        CPPUNIT_ASSERT(!pDPObj->GetHeaderLayout());
        // Check whether the row header has the right popupbutton flag
        const ScPatternAttr* pPattern = pDoc->GetPattern(0, 2, 0);
        const SfxPoolItem& rPoolItem = pPattern->GetItem(ATTR_MERGE_FLAG);
        const ScMergeFlagAttr& rMergeFlag = static_cast<const ScMergeFlagAttr&>(rPoolItem);
        CPPUNIT_ASSERT(rMergeFlag.GetValue() & ScMF::ButtonPopup);
    }

    // Check also after a reload
    saveAndReload("MS Excel 97");
    pDoc = getScDoc();

    // There should be exactly 2 pivot tables
    pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), pDPs->GetCount());

    // First table has extra row header
    {
        const ScDPObject* pDPObj = &(*pDPs)[0];
        CPPUNIT_ASSERT_MESSAGE("Failed to get a pivot table object.", pDPObj);
        CPPUNIT_ASSERT(pDPObj->GetHeaderLayout());
        // Check whether the row header has the right popupbutton flag
        const ScPatternAttr* pPattern = pDoc->GetPattern(3, 3, 0);
        const SfxPoolItem& rPoolItem = pPattern->GetItem(ATTR_MERGE_FLAG);
        const ScMergeFlagAttr& rMergeFlag = static_cast<const ScMergeFlagAttr&>(rPoolItem);
        CPPUNIT_ASSERT(rMergeFlag.GetValue() & ScMF::ButtonPopup);
    }

    // Second table has no extra row header
    {
        const ScDPObject* pDPObj = &(*pDPs)[1];
        CPPUNIT_ASSERT_MESSAGE("Failed to get a pivot table object.", pDPObj);
        CPPUNIT_ASSERT(!pDPObj->GetHeaderLayout());
        // Check whether the row header has the right popupbutton flag
        const ScPatternAttr* pPattern = pDoc->GetPattern(0, 2, 0);
        const SfxPoolItem& rPoolItem = pPattern->GetItem(ATTR_MERGE_FLAG);
        const ScMergeFlagAttr& rMergeFlag = static_cast<const ScMergeFlagAttr&>(rPoolItem);
        CPPUNIT_ASSERT(rMergeFlag.GetValue() & ScMF::ButtonPopup);
    }
}

void ScPivotTableFiltersTest::testPivotTableDoubleFieldFilter()
{
    createScDoc("xls/pivottable_double_field_filter.xls");
    ScDocument* pDoc = getScDoc();
    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), pDPs->GetCount());

    // Reload and check filtering of row dimensions
    saveAndReload("MS Excel 97");
    pDoc = getScDoc();
    pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), pDPs->GetCount());

    // Field with general formatting
    {
        const ScDPObject* pDPObj = &(*pDPs)[2];
        CPPUNIT_ASSERT(pDPObj);
        ScDPSaveData* pSaveData = pDPObj->GetSaveData();
        CPPUNIT_ASSERT(pSaveData);
        ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Double field1");
        CPPUNIT_ASSERT(pSaveDim);

        const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
        CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
        auto aIter = rMembers.begin();
        ScDPSaveMember* pMember = *aIter; // "1"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(!pMember->GetIsVisible());
        ++aIter;
        pMember = *aIter; // "2"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        ++aIter;
        pMember = *aIter; // "3"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
    }

    // Number formatting
    {
        const ScDPObject* pDPObj = &(*pDPs)[1];
        CPPUNIT_ASSERT(pDPObj);
        ScDPSaveData* pSaveData = pDPObj->GetSaveData();
        CPPUNIT_ASSERT(pSaveData);
        ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Double field2");
        CPPUNIT_ASSERT(pSaveDim);

        const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
        CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
        auto aIter = rMembers.begin();
        ScDPSaveMember* pMember = *aIter; // "1.00"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        ++aIter;
        pMember = *aIter; // "2.00"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(!pMember->GetIsVisible());
        ++aIter;
        pMember = *aIter; // "3.00"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(!pMember->GetIsVisible());
    }

    // With thousand separator
    {
        const ScDPObject* pDPObj = &(*pDPs)[0];
        CPPUNIT_ASSERT(pDPObj);
        ScDPSaveData* pSaveData = pDPObj->GetSaveData();
        CPPUNIT_ASSERT(pSaveData);
        ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Double field3");
        CPPUNIT_ASSERT(pSaveDim);

        const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
        CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
        auto aIter = rMembers.begin();
        ScDPSaveMember* pMember = *aIter; // "10,000.00"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        ++aIter;
        pMember = *aIter; // "20,000.00"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(!pMember->GetIsVisible());
        ++aIter;
        pMember = *aIter; // "30,000.00"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
    }
}

void ScPivotTableFiltersTest::testPivotTableStringFieldFilter()
{
    createScDoc("xls/pivottable_string_field_filter.xls");
    ScDocument* pDoc = getScDoc();
    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());

    // Reload and check filtering of row dimensions
    saveAndReload("MS Excel 97");
    pDoc = getScDoc();
    pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());

    const ScDPObject* pDPObj = &(*pDPs)[0];
    CPPUNIT_ASSERT(pDPObj);
    ScDPSaveData* pSaveData = pDPObj->GetSaveData();
    CPPUNIT_ASSERT(pSaveData);
    ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Country");
    CPPUNIT_ASSERT(pSaveDim);

    const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
    CPPUNIT_ASSERT_EQUAL(size_t(2), rMembers.size());
    ScDPSaveMember* pMember = pSaveDim->GetExistingMemberByName("United Kingdom");
    CPPUNIT_ASSERT(pMember);
    CPPUNIT_ASSERT(pMember->HasIsVisible());
    CPPUNIT_ASSERT(!pMember->GetIsVisible());
    pMember = pSaveDim->GetExistingMemberByName("United States");
    CPPUNIT_ASSERT(pMember);
    CPPUNIT_ASSERT(pMember->HasIsVisible());
    CPPUNIT_ASSERT(pMember->GetIsVisible());
}

void ScPivotTableFiltersTest::testPivotTableDateFieldFilter()
{
    createScDoc("xls/pivottable_date_field_filter.xls");
    ScDocument* pDoc = getScDoc();
    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());

    // Reload and check filtering of row dimensions
    saveAndReload("MS Excel 97");
    pDoc = getScDoc();
    pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());
    const ScDPObject* pDPObj = &(*pDPs)[0];
    CPPUNIT_ASSERT(pDPObj);
    ScDPSaveData* pSaveData = pDPObj->GetSaveData();
    CPPUNIT_ASSERT(pSaveData);

    {
        ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Date");
        CPPUNIT_ASSERT(pSaveDim);

        const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
        CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
        auto aIter = rMembers.begin();
        ScDPSaveMember* pMember = *aIter; // "2016. január 6."
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        ++aIter;
        pMember = *aIter; // "2016. január 7."
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        ++aIter;
        pMember = *aIter; // "2016. január 8."
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(!pMember->GetIsVisible());
    }

    {
        ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Date2");
        CPPUNIT_ASSERT(pSaveDim);

        const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
        CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
        auto aIter = rMembers.begin();
        ScDPSaveMember* pMember = *aIter; // "2016-01-06"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        ++aIter;
        pMember = *aIter; // "2016-01-07"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        ++aIter;
        pMember = *aIter; // "2016-01-08"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(!pMember->GetIsVisible());
    }

    {
        ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Date3");
        CPPUNIT_ASSERT(pSaveDim);

        const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
        CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
        auto aIter = rMembers.begin();
        ScDPSaveMember* pMember = *aIter; // "2016. 1. 6. 0:00"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        ++aIter;
        pMember = *aIter; // "2016. 1. 7. 0:00"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(!pMember->GetIsVisible());
        ++aIter;
        pMember = *aIter; // "2016. 1. 8. 0:00"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
    }
}

void ScPivotTableFiltersTest::testPivotTableBoolFieldFilter()
{
    createScDoc("xls/pivottable_bool_field_filter.xls");
    ScDocument* pDoc = getScDoc();
    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());

    // Reload and check filtering of row dimensions
    saveAndReload("MS Excel 97");
    pDoc = getScDoc();
    pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());
    const ScDPObject* pDPObj = &(*pDPs)[0];
    CPPUNIT_ASSERT(pDPObj);
    ScDPSaveData* pSaveData = pDPObj->GetSaveData();
    CPPUNIT_ASSERT(pSaveData);

    ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Bool field");
    CPPUNIT_ASSERT(pSaveDim);

    const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
    CPPUNIT_ASSERT_EQUAL(size_t(2), rMembers.size());
    ScDPSaveMember* pMember = pSaveDim->GetExistingMemberByName("0");
    CPPUNIT_ASSERT(pMember);
    CPPUNIT_ASSERT(pMember->HasIsVisible());
    CPPUNIT_ASSERT(!pMember->GetIsVisible());
    pMember = pSaveDim->GetExistingMemberByName("1");
    CPPUNIT_ASSERT(pMember);
    CPPUNIT_ASSERT(pMember->HasIsVisible());
    CPPUNIT_ASSERT(pMember->GetIsVisible());
}

void ScPivotTableFiltersTest::testPivotTableRowColPageFieldFilter()
{
    createScDoc("xls/pivottable_rowcolpage_field_filter.xls");
    ScDocument* pDoc = getScDoc();
    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());

    // Reload and check filtering of row dimensions
    saveAndReload("MS Excel 97");
    pDoc = getScDoc();
    pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());
    const ScDPObject* pDPObj = &(*pDPs)[0];
    CPPUNIT_ASSERT(pDPObj);
    ScDPSaveData* pSaveData = pDPObj->GetSaveData();
    CPPUNIT_ASSERT(pSaveData);

    // Row field
    {
        ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Order ID");
        CPPUNIT_ASSERT(pSaveDim);
        CPPUNIT_ASSERT_EQUAL(sheet::DataPilotFieldOrientation_ROW, pSaveDim->GetOrientation());

        const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
        CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
        ScDPSaveMember* pMember = pSaveDim->GetExistingMemberByName("1");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        pMember = pSaveDim->GetExistingMemberByName("2");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        pMember = pSaveDim->GetExistingMemberByName("3");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(!pMember->GetIsVisible());
    }

    // Column field
    {
        ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Double2 field");
        CPPUNIT_ASSERT(pSaveDim);
        CPPUNIT_ASSERT_EQUAL(sheet::DataPilotFieldOrientation_COLUMN, pSaveDim->GetOrientation());

        const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
        CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
        ScDPSaveMember* pMember = pSaveDim->GetExistingMemberByName("2");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        pMember = pSaveDim->GetExistingMemberByName("3");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        pMember = pSaveDim->GetExistingMemberByName("4");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(!pMember->GetIsVisible());
    }

    // Page field
    {
        ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Double3 field");
        CPPUNIT_ASSERT(pSaveDim);
        CPPUNIT_ASSERT_EQUAL(sheet::DataPilotFieldOrientation_PAGE, pSaveDim->GetOrientation());

        const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
        CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
        ScDPSaveMember* pMember = pSaveDim->GetExistingMemberByName("5");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        pMember = pSaveDim->GetExistingMemberByName("6");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(!pMember->GetIsVisible());
        pMember = pSaveDim->GetExistingMemberByName("7");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
    }

    // Hidden field
    {
        ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Double4 field");
        CPPUNIT_ASSERT(pSaveDim);
        CPPUNIT_ASSERT_EQUAL(sheet::DataPilotFieldOrientation_HIDDEN, pSaveDim->GetOrientation());

        const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
        CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
        ScDPSaveMember* pMember = pSaveDim->GetExistingMemberByName("8");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(!pMember->GetIsVisible());
        pMember = pSaveDim->GetExistingMemberByName("9");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        pMember = pSaveDim->GetExistingMemberByName("10");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
    }
}

void ScPivotTableFiltersTest::testPivotTableEmptyItem()
{
    createScDoc("xls/pivottable_empty_item.xls");
    ScDocument* pDoc = getScDoc();
    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());

    // Reload and check filtering of row dimensions
    saveAndReload("MS Excel 97");
    pDoc = getScDoc();
    pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());
    const ScDPObject* pDPObj = &(*pDPs)[0];
    CPPUNIT_ASSERT(pDPObj);
    ScDPSaveData* pSaveData = pDPObj->GetSaveData();
    CPPUNIT_ASSERT(pSaveData);

    ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Category");
    CPPUNIT_ASSERT(pSaveDim);

    const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
    CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
    ScDPSaveMember* pMember = pSaveDim->GetExistingMemberByName("Fruit");
    CPPUNIT_ASSERT(pMember);
    CPPUNIT_ASSERT(pMember->HasIsVisible());
    CPPUNIT_ASSERT(pMember->GetIsVisible());
    pMember = pSaveDim->GetExistingMemberByName("Vegetables");
    CPPUNIT_ASSERT(pMember);
    CPPUNIT_ASSERT(pMember->HasIsVisible());
    CPPUNIT_ASSERT(!pMember->GetIsVisible());
    pMember = pSaveDim->GetExistingMemberByName("");
    CPPUNIT_ASSERT(pMember);
    CPPUNIT_ASSERT(pMember->HasIsVisible());
    CPPUNIT_ASSERT(!pMember->GetIsVisible());
}

void ScPivotTableFiltersTest::testPivotTablePageFieldFilter()
{
    createScDoc("xls/pivottable_page_field_filter.xls");
    ScDocument* pDoc = getScDoc();
    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());

    // Reload and check filtering of row dimensions
    saveAndReload("MS Excel 97");
    pDoc = getScDoc();
    pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());
    const ScDPObject* pDPObj = &(*pDPs)[0];
    CPPUNIT_ASSERT(pDPObj);
    ScDPSaveData* pSaveData = pDPObj->GetSaveData();
    CPPUNIT_ASSERT(pSaveData);

    // Only one item is selected (an empty item)
    {
        ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Product");
        CPPUNIT_ASSERT(pSaveDim);
        const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
        CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
        ScDPSaveMember* pMember = pSaveDim->GetExistingMemberByName("Banana");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(!pMember->GetIsVisible());
        pMember = pSaveDim->GetExistingMemberByName("Carrots");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(!pMember->GetIsVisible());
        pMember = pSaveDim->GetExistingMemberByName("");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
    }

    // More items are selected, but not all
    {
        ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Category");
        CPPUNIT_ASSERT(pSaveDim);

        const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
        CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
        ScDPSaveMember* pMember = pSaveDim->GetExistingMemberByName("Vegetables");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(!pMember->GetIsVisible());
        pMember = pSaveDim->GetExistingMemberByName("Fruit");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        pMember = pSaveDim->GetExistingMemberByName("");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
    }

    // All items are selected
    {
        ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Order ID");
        CPPUNIT_ASSERT(pSaveDim);

        const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
        CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
        ScDPSaveMember* pMember = pSaveDim->GetExistingMemberByName("1");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        pMember = pSaveDim->GetExistingMemberByName("2");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        pMember = pSaveDim->GetExistingMemberByName("3");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
    }

    // Only one item is selected (a non-empty item)
    {
        ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Country");
        CPPUNIT_ASSERT(pSaveDim);

        const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
        CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
        ScDPSaveMember* pMember = pSaveDim->GetExistingMemberByName("United States");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(!pMember->GetIsVisible());
        pMember = pSaveDim->GetExistingMemberByName("United Kingdom");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        pMember = pSaveDim->GetExistingMemberByName("");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(!pMember->GetIsVisible());
    }
}

void ScPivotTableFiltersTest::testPivotTableFirstHeaderRowXLSX()
{
    // tdf#112733: We have different tables here, but have the same value as firstHeaderRow
    // The documentation is not clear about what firstHeaderRow actually means, but MS Excel works on this way
    createScDoc("xlsx/pivot_table_first_header_row.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pTable = parseExport("xl/pivotTables/pivotTable1.xml");
    CPPUNIT_ASSERT(pTable);
    assertXPath(pTable, "/x:pivotTableDefinition/x:location", "firstHeaderRow", "1");

    pTable = parseExport("xl/pivotTables/pivotTable2.xml");
    CPPUNIT_ASSERT(pTable);
    assertXPath(pTable, "/x:pivotTableDefinition/x:location", "firstHeaderRow", "1");

    pTable = parseExport("xl/pivotTables/pivotTable3.xml");
    CPPUNIT_ASSERT(pTable);
    assertXPath(pTable, "/x:pivotTableDefinition/x:location", "firstHeaderRow", "1");
}

void ScPivotTableFiltersTest::testPivotTableDoubleFieldFilterXLSX()
{
    createScDoc("xlsx/pivottable_double_field_filter.xlsx");
    ScDocument* pDoc = getScDoc();
    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), pDPs->GetCount());

    // Reload and check filtering of row dimensions
    saveAndReload("Calc Office Open XML");
    pDoc = getScDoc();
    pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), pDPs->GetCount());

    // Field with general formatting
    {
        const ScDPObject* pDPObj = &(*pDPs)[0];
        CPPUNIT_ASSERT(pDPObj);
        ScDPSaveData* pSaveData = pDPObj->GetSaveData();
        CPPUNIT_ASSERT(pSaveData);
        ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Double field1");
        CPPUNIT_ASSERT(pSaveDim);

        const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
        CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
        auto aIter = rMembers.begin();
        ScDPSaveMember* pMember = *aIter; // "1"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(!pMember->GetIsVisible());
        ++aIter;
        pMember = *aIter; // "2"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        ++aIter;
        pMember = *aIter; // "3"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
    }

    // Number formatting
    {
        const ScDPObject* pDPObj = &(*pDPs)[1];
        CPPUNIT_ASSERT(pDPObj);
        ScDPSaveData* pSaveData = pDPObj->GetSaveData();
        CPPUNIT_ASSERT(pSaveData);
        ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Double field2");
        CPPUNIT_ASSERT(pSaveDim);

        const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
        CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
        auto aIter = rMembers.begin();
        ScDPSaveMember* pMember = *aIter; // "1.00"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        ++aIter;
        pMember = *aIter; // "2.00"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(!pMember->GetIsVisible());
        ++aIter;
        pMember = *aIter; // "3.00"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(!pMember->GetIsVisible());
    }

    // With thousand separator
    {
        const ScDPObject* pDPObj = &(*pDPs)[2];
        CPPUNIT_ASSERT(pDPObj);
        ScDPSaveData* pSaveData = pDPObj->GetSaveData();
        CPPUNIT_ASSERT(pSaveData);
        ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Double field3");
        CPPUNIT_ASSERT(pSaveDim);

        const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
        CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
        auto aIter = rMembers.begin();
        ScDPSaveMember* pMember = *aIter; // "10,000.00"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        ++aIter;
        pMember = *aIter; // "20,000.00"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(!pMember->GetIsVisible());
        ++aIter;
        pMember = *aIter; // "30,000.00"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
    }
}

void ScPivotTableFiltersTest::testPivotTableStringFieldFilterXLSX()
{
    createScDoc("xlsx/pivottable_string_field_filter.xlsx");
    ScDocument* pDoc = getScDoc();
    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());

    // Reload and check filtering of row dimensions
    saveAndReload("Calc Office Open XML");
    pDoc = getScDoc();
    pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());

    const ScDPObject* pDPObj = &(*pDPs)[0];
    CPPUNIT_ASSERT(pDPObj);
    ScDPSaveData* pSaveData = pDPObj->GetSaveData();
    CPPUNIT_ASSERT(pSaveData);
    ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Country");
    CPPUNIT_ASSERT(pSaveDim);

    const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
    CPPUNIT_ASSERT_EQUAL(size_t(2), rMembers.size());
    ScDPSaveMember* pMember = pSaveDim->GetExistingMemberByName("United Kingdom");
    CPPUNIT_ASSERT(pMember);
    CPPUNIT_ASSERT(pMember->HasIsVisible());
    CPPUNIT_ASSERT(!pMember->GetIsVisible());
    pMember = pSaveDim->GetExistingMemberByName("United States");
    CPPUNIT_ASSERT(pMember);
    CPPUNIT_ASSERT(pMember->HasIsVisible());
    CPPUNIT_ASSERT(pMember->GetIsVisible());
}

void ScPivotTableFiltersTest::testPivotTableDateFieldFilterXLSX()
{
    createScDoc("xlsx/pivottable_date_field_filter.xlsx");
    ScDocument* pDoc = getScDoc();
    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());

    // Reload and check filtering of row dimensions
    saveAndReload("Calc Office Open XML");
    pDoc = getScDoc();
    pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());
    const ScDPObject* pDPObj = &(*pDPs)[0];
    CPPUNIT_ASSERT(pDPObj);
    ScDPSaveData* pSaveData = pDPObj->GetSaveData();
    CPPUNIT_ASSERT(pSaveData);

    {
        ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Date");
        CPPUNIT_ASSERT(pSaveDim);

        const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
        //CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
        auto aIter = rMembers.begin();
        ScDPSaveMember* pMember = *aIter; // "2016. január 7."
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        ++aIter;
        pMember = *aIter; // "2016. január 8."
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
    }

    {
        ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Date2");
        CPPUNIT_ASSERT(pSaveDim);

        const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
        CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
        auto aIter = rMembers.begin();
        ScDPSaveMember* pMember = *aIter; // "2016-01-07"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        ++aIter;
        pMember = *aIter; // "2016-01-08"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
    }

    {
        ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Date3");
        CPPUNIT_ASSERT(pSaveDim);

        const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
        CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
        auto aIter = rMembers.begin();
        ScDPSaveMember* pMember = *aIter; // "2016. 1. 7. 0:00"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        ++aIter;
        pMember = *aIter; // "2016. 1. 8. 0:00"
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(!pMember->GetIsVisible());
    }
}

void ScPivotTableFiltersTest::testPivotTableBoolFieldFilterXLSX()
{
    createScDoc("xlsx/pivottable_bool_field_filter.xlsx");
    ScDocument* pDoc = getScDoc();
    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());
    CPPUNIT_ASSERT_EQUAL(OUString("TRUE"), pDoc->GetString(ScAddress(0, 1, 0))); //A2

    // Reload and check filtering of row dimensions
    saveAndReload("Calc Office Open XML");
    pDoc = getScDoc();
    pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());
    const ScDPObject* pDPObj = &(*pDPs)[0];
    CPPUNIT_ASSERT(pDPObj);
    ScDPSaveData* pSaveData = pDPObj->GetSaveData();
    CPPUNIT_ASSERT(pSaveData);

    ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Bool field");
    CPPUNIT_ASSERT(pSaveDim);

    const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
    CPPUNIT_ASSERT_EQUAL(size_t(2), rMembers.size());
    ScDPSaveMember* pMember = pSaveDim->GetExistingMemberByName("FALSE");
    CPPUNIT_ASSERT(pMember);
    CPPUNIT_ASSERT(pMember->HasIsVisible());
    CPPUNIT_ASSERT(!pMember->GetIsVisible());
    pMember = pSaveDim->GetExistingMemberByName("TRUE");
    CPPUNIT_ASSERT(pMember);
    CPPUNIT_ASSERT(pMember->HasIsVisible());
    CPPUNIT_ASSERT(pMember->GetIsVisible());
}

void ScPivotTableFiltersTest::testPivotTableRowColPageFieldFilterXLSX()
{
    createScDoc("xlsx/pivottable_rowcolpage_field_filter.xlsx");
    ScDocument* pDoc = getScDoc();
    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());

    // Reload and check filtering of row dimensions
    saveAndReload("Calc Office Open XML");
    pDoc = getScDoc();
    pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());
    const ScDPObject* pDPObj = &(*pDPs)[0];
    CPPUNIT_ASSERT(pDPObj);
    ScDPSaveData* pSaveData = pDPObj->GetSaveData();
    CPPUNIT_ASSERT(pSaveData);

    // Row field
    {
        ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Order ID");
        CPPUNIT_ASSERT(pSaveDim);
        CPPUNIT_ASSERT_EQUAL(sheet::DataPilotFieldOrientation_ROW, pSaveDim->GetOrientation());

        const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
        CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
        ScDPSaveMember* pMember = pSaveDim->GetExistingMemberByName("1");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        pMember = pSaveDim->GetExistingMemberByName("2");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        pMember = pSaveDim->GetExistingMemberByName("3");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(!pMember->GetIsVisible());
    }

    // Column field
    {
        ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Double2 field");
        CPPUNIT_ASSERT(pSaveDim);
        CPPUNIT_ASSERT_EQUAL(sheet::DataPilotFieldOrientation_COLUMN, pSaveDim->GetOrientation());

        const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
        CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
        ScDPSaveMember* pMember = pSaveDim->GetExistingMemberByName("2");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        pMember = pSaveDim->GetExistingMemberByName("3");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        pMember = pSaveDim->GetExistingMemberByName("4");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(!pMember->GetIsVisible());
    }

    // Page field
    {
        ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"Double3 field");
        CPPUNIT_ASSERT(pSaveDim);
        CPPUNIT_ASSERT_EQUAL(sheet::DataPilotFieldOrientation_PAGE, pSaveDim->GetOrientation());

        const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
        CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
        ScDPSaveMember* pMember = pSaveDim->GetExistingMemberByName("5");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
        pMember = pSaveDim->GetExistingMemberByName("6");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(!pMember->GetIsVisible());
        pMember = pSaveDim->GetExistingMemberByName("7");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible());
        CPPUNIT_ASSERT(pMember->GetIsVisible());
    }

    // Hidden field
    /* TODO
    {
        ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName("Double4 field");
        CPPUNIT_ASSERT(pSaveDim);
        CPPUNIT_ASSERT_EQUAL(sheet::DataPilotFieldOrientation_HIDDEN, pSaveDim->GetOrientation());

        const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
        CPPUNIT_ASSERT_EQUAL(size_t(3), rMembers.size());
        ScDPSaveMember* pMember = pSaveDim->GetExistingMemberByName("8");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible() && !pMember->GetIsVisible());
        pMember = pSaveDim->GetExistingMemberByName("9");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible() && pMember->GetIsVisible());
        pMember = pSaveDim->GetExistingMemberByName("10");
        CPPUNIT_ASSERT(pMember);
        CPPUNIT_ASSERT(pMember->HasIsVisible() && pMember->GetIsVisible());
    }*/
}

void ScPivotTableFiltersTest::testPivotTableErrorItemFilterXLSX()
{
    createScDoc("xlsx/pivottable_error_item_filter.xlsx");
    ScDocument* pDoc = getScDoc();
    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());

    // Reload and check whether filtering is preserved
    saveAndReload("Calc Office Open XML");
    pDoc = getScDoc();
    pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());
    const ScDPObject* pDPObj = &(*pDPs)[0];
    CPPUNIT_ASSERT(pDPObj);
    ScDPSaveData* pSaveData = pDPObj->GetSaveData();
    CPPUNIT_ASSERT(pSaveData);

    ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"b");
    CPPUNIT_ASSERT(pSaveDim);
    const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
    CPPUNIT_ASSERT_EQUAL(size_t(4), rMembers.size());
    ScDPSaveMember* pMember = pSaveDim->GetExistingMemberByName("#DIV/0!");
    CPPUNIT_ASSERT(pMember);
    CPPUNIT_ASSERT(pMember->HasIsVisible());
    CPPUNIT_ASSERT(!pMember->GetIsVisible());
}

void ScPivotTableFiltersTest::testPivotTableErrorItemFilterXLSB()
{
    createScDoc("xlsb/pivottable_error_item_filter.xlsb");
    ScDocument* pDoc = getScDoc();
    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());
    const ScDPObject* pDPObj = &(*pDPs)[0];
    CPPUNIT_ASSERT(pDPObj);
    ScDPSaveData* pSaveData = pDPObj->GetSaveData();
    CPPUNIT_ASSERT(pSaveData);

    ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"b");
    CPPUNIT_ASSERT(pSaveDim);
    const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
    CPPUNIT_ASSERT_EQUAL(size_t(4), rMembers.size());
    ScDPSaveMember* pMember = pSaveDim->GetExistingMemberByName("#DIV/0!");
    CPPUNIT_ASSERT(pMember);
    CPPUNIT_ASSERT(pMember->HasIsVisible());
    CPPUNIT_ASSERT(!pMember->GetIsVisible());
}

void ScPivotTableFiltersTest::testPivotTableErrorItem2FilterXLSX()
{
    createScDoc("xlsx/tdf122471.xlsx");
    ScDocument* pDoc = getScDoc();
    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());

    // Reload and check whether filtering is preserved
    saveAndReload("Calc Office Open XML");
    pDoc = getScDoc();
    pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());
    const ScDPObject* pDPObj = &(*pDPs)[0];
    CPPUNIT_ASSERT(pDPObj);
    ScDPSaveData* pSaveData = pDPObj->GetSaveData();
    CPPUNIT_ASSERT(pSaveData);

    ScDPSaveDimension* pSaveDim = pSaveData->GetExistingDimensionByName(u"PPP");
    CPPUNIT_ASSERT(pSaveDim);
    const ScDPSaveDimension::MemberList& rMembers = pSaveDim->GetMembers();
    // prior to the patch, columns were missing due to an exception dropping the column data
    CPPUNIT_ASSERT_EQUAL(size_t(21), rMembers.size());
}

void ScPivotTableFiltersTest::testPivotTableOutlineModeXLSX()
{
    createScDoc("xlsx/pivottable_outline_mode.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pTable = parseExport("xl/pivotTables/pivotTable1.xml");
    CPPUNIT_ASSERT(pTable);

    // Next to the outline flags, compact flags also should be set (true is the default)
    assertXPath(pTable, "/x:pivotTableDefinition", "outline", "1");
    assertXPath(pTable, "/x:pivotTableDefinition", "outlineData", "1");
    assertXPath(pTable, "/x:pivotTableDefinition", "compact", "0");
    assertXPath(pTable, "/x:pivotTableDefinition", "compactData", "0");
    assertXPath(pTable, "/x:pivotTableDefinition/x:pivotFields/x:pivotField[1]", "compact", "0");
}

void ScPivotTableFiltersTest::testPivotTableDuplicatedMemberFilterXLSX()
{
    createScDoc("xlsx/pivottable_duplicated_member_filter.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pTable = parseExport("xl/pivotTables/pivotTable1.xml");
    CPPUNIT_ASSERT(pTable);

    // Check whether page field has the right number of items
    assertXPath(pTable, "/x:pivotTableDefinition/x:pivotFields/x:pivotField[5]", "axis",
                "axisPage");
    assertXPath(pTable, "/x:pivotTableDefinition/x:pivotFields/x:pivotField[5]/x:items", "count",
                "21");
}

void ScPivotTableFiltersTest::testPivotTableTabularModeXLSX()
{
    createScDoc("xlsx/pivottable_tabular_mode.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pTable = parseExport("xl/pivotTables/pivotTable1.xml");
    CPPUNIT_ASSERT(pTable);

    // In tabular mode both outline and compact flag should be false
    assertXPath(pTable, "/x:pivotTableDefinition", "outline", "0");
    assertXPath(pTable, "/x:pivotTableDefinition", "outlineData", "0");
    assertXPath(pTable, "/x:pivotTableDefinition", "compact", "0");
    assertXPath(pTable, "/x:pivotTableDefinition", "compactData", "0");
    assertXPath(pTable, "/x:pivotTableDefinition/x:pivotFields/x:pivotField[1]", "compact", "0");
    assertXPath(pTable, "/x:pivotTableDefinition/x:pivotFields/x:pivotField[1]", "outline", "0");
}

void ScPivotTableFiltersTest::testPivotTableDuplicateFields()
{
    createScDoc("ods/caseinsensitive-duplicate-fields.ods");

    save("Calc Office Open XML");
    xmlDocUniquePtr pCacheDef = parseExport("xl/pivotCache/pivotCacheDefinition1.xml");
    CPPUNIT_ASSERT(pCacheDef);

    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields", "count", "6");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[1]", "name", "ID");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[2]", "name", "Name");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[3]", "name",
                "Score");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[4]", "name",
                "Method");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[5]", "name",
                "method2");
    assertXPath(pCacheDef, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[6]", "name",
                "Method3");
}

void ScPivotTableFiltersTest::testTdf112106()
{
    createScDoc("xlsx/tdf112106.xlsx");
    ScDocument* pDoc = getScDoc();
    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());

    // Reload and check data layout dim
    saveAndReload("MS Excel 97");
    pDoc = getScDoc();
    pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pDPs->GetCount());
    const ScDPObject* pDPObj = &(*pDPs)[0];
    CPPUNIT_ASSERT(pDPObj);
    const ScDPSaveData* pSaveData = pDPObj->GetSaveData();
    CPPUNIT_ASSERT(pSaveData);

    // Check that we have an existing data layout dimension
    const ScDPSaveDimension* pDim = pSaveData->GetExistingDataLayoutDimension();
    CPPUNIT_ASSERT(pDim);
    const std::optional<OUString>& pLayoutName = pDim->GetLayoutName();
    CPPUNIT_ASSERT(pLayoutName);
    CPPUNIT_ASSERT_EQUAL(ScResId(STR_PIVOT_DATA), (*pLayoutName));
}

void ScPivotTableFiltersTest::testTdf123923()
{
    // tdf#123923: Excel fails when it finds "Err:504" instead of "#REF!" in pivot table cache

    createScDoc("ods/pivot-table-err-in-cache.ods");

    save("Calc Office Open XML");
    xmlDocUniquePtr pTable = parseExport("xl/pivotCache/pivotCacheDefinition1.xml");
    CPPUNIT_ASSERT(pTable);

    assertXPath(pTable, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[1]/x:sharedItems/x:e",
                "v", "#REF!");
}

void ScPivotTableFiltersTest::testTdf123939()
{
    // tdf#123939: Excel warns on containsMixedTypes="1" if sharedItems has only strings and errors

    createScDoc("ods/pivot-table-str-and-err-in-data.ods");

    save("Calc Office Open XML");
    xmlDocUniquePtr pTable = parseExport("xl/pivotCache/pivotCacheDefinition1.xml");
    CPPUNIT_ASSERT(pTable);

    assertXPathNoAttribute(pTable,
                           "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[1]/x:sharedItems",
                           "containsMixedTypes");

    // But we must emit containsMixedTypes="1" for a mix of errors and non-string types!

    pTable = parseExport("xl/pivotCache/pivotCacheDefinition2.xml");
    CPPUNIT_ASSERT(pTable);

    assertXPath(pTable, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[1]/x:sharedItems",
                "containsMixedTypes", "1");
}

void ScPivotTableFiltersTest::testTdf124651()
{
    createScDoc("ods/tdf124651_simplePivotTable.ods");

    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/pivotTables/pivotTable1.xml");
    CPPUNIT_ASSERT(pDoc);
    // We have to export name attribute, even though it's optional according to ECMA-376 standard,
    // because Excel (at least 2016) seems to require it.
    // tdf#124881: this shouldn't be an empty string
    assertXPath(pDoc, "/x:pivotTableDefinition/x:dataFields/x:dataField", "name", "Sum - num");
}

void ScPivotTableFiltersTest::testTdf124736()
{
    createScDoc("xlsx/pivot-table/shared-dategroup.xlsx");

    save("Calc Office Open XML");

    xmlDocUniquePtr pTable = parseExport("xl/pivotCache/pivotCacheDefinition1.xml");
    CPPUNIT_ASSERT(pTable);

    assertXPath(pTable,
                "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[1]/x:fieldGroup/x:groupItems",
                "count", "45");
    // Group items must start with "<05/16/1958", then years sorted ascending, then ">06/11/2009"
    // They used to have years in the beginning, then "<05/16/1958", then ">06/11/2009".
    // The "<" and ">" date strings are locale-dependent, so test depends on en_US locale
    assertXPath(
        pTable,
        "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[1]/x:fieldGroup/x:groupItems/x:s[1]",
        "v", "<05/16/1958");
    for (int i = 2; i <= 44; ++i)
        assertXPath(
            pTable,
            "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[1]/x:fieldGroup/x:groupItems/x:s["
                + OString::number(i) + "]",
            "v", OUString::number(1963 + i));
    assertXPath(
        pTable,
        "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[1]/x:fieldGroup/x:groupItems/x:s[45]",
        "v", ">06/11/2009");

    // Now check that table references these in correct order (document-dependent, so this is how
    // it should be in this specific testdoc which shows "<" and ">" values in the end)
    pTable = parseExport("xl/pivotTables/pivotTable1.xml");
    CPPUNIT_ASSERT(pTable);
    assertXPath(pTable, "/x:pivotTableDefinition/x:pivotFields/x:pivotField[1]/x:items", "count",
                "46");
    const int vals[] = { 1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
                         16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
                         31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 0,  44 };
    for (size_t i = 0; i < SAL_N_ELEMENTS(vals); ++i)
    {
        assertXPath(pTable,
                    "/x:pivotTableDefinition/x:pivotFields/x:pivotField[1]/x:items/x:item["
                        + OString::number(i + 1) + "]",
                    "x", OUString::number(vals[i]));
    }
    assertXPath(pTable, "/x:pivotTableDefinition/x:pivotFields/x:pivotField[1]/x:items/x:item[46]",
                "t", "default");
}

void ScPivotTableFiltersTest::tesTtdf124772NumFmt()
{
    createScDoc("ods/pivot-table-num-fmt.ods");

    save("Calc Office Open XML");

    xmlDocUniquePtr pTable = parseExport("xl/pivotTables/pivotTable1.xml");
    CPPUNIT_ASSERT(pTable);

    // This asserts that numFmtId attribute is present
    const OUString sXclNumFmt
        = getXPath(pTable, "/x:pivotTableDefinition/x:dataFields/x:dataField", "numFmtId");

    pTable = parseExport("xl/styles.xml");
    CPPUNIT_ASSERT(pTable);

    // Check that we refer to correct format
    assertXPath(pTable, "/x:styleSheet/x:numFmts/x:numFmt[@numFmtId='" + sXclNumFmt.toUtf8() + "']",
                "formatCode", "\\$#,##0");
}

void ScPivotTableFiltersTest::testTdf124810()
{
    {
        // First, test that we roundtrip existing pivot table style information from XLSX.
        createScDoc("xlsx/pivot_dark1.xlsx");

        save("Calc Office Open XML");
        xmlDocUniquePtr pTable = parseExport("xl/pivotTables/pivotTable1.xml");
        CPPUNIT_ASSERT(pTable);

        // All attributes must have been roundtripped correctly (testdoc uses some non-default values)
        assertXPath(pTable, "/x:pivotTableDefinition/x:pivotTableStyleInfo", "name",
                    "PivotStyleDark1");
        assertXPath(pTable, "/x:pivotTableDefinition/x:pivotTableStyleInfo", "showRowHeaders", "1");
        assertXPath(pTable, "/x:pivotTableDefinition/x:pivotTableStyleInfo", "showColHeaders", "1");
        assertXPath(pTable, "/x:pivotTableDefinition/x:pivotTableStyleInfo", "showRowStripes", "1");
        assertXPath(pTable, "/x:pivotTableDefinition/x:pivotTableStyleInfo", "showColStripes", "0");
        assertXPath(pTable, "/x:pivotTableDefinition/x:pivotTableStyleInfo", "showLastColumn", "0");
    }

    {
        // Now check that we export default style information when there's no such information in
        // original document. Just use some ODS as source. This might be changed when we start
        // exporting better pivot table style information.
        createScDoc("ods/tdf124651_simplePivotTable.ods");

        save("Calc Office Open XML");
        xmlDocUniquePtr pTable = parseExport("xl/pivotTables/pivotTable1.xml");
        CPPUNIT_ASSERT(pTable);

        // The default style for pivot tables in Excel 2007 through 2016 is PivotStyleLight16
        assertXPath(pTable, "/x:pivotTableDefinition/x:pivotTableStyleInfo", "name",
                    "PivotStyleLight16");
        assertXPath(pTable, "/x:pivotTableDefinition/x:pivotTableStyleInfo", "showRowHeaders", "1");
        assertXPath(pTable, "/x:pivotTableDefinition/x:pivotTableStyleInfo", "showColHeaders", "1");
        assertXPath(pTable, "/x:pivotTableDefinition/x:pivotTableStyleInfo", "showRowStripes", "0");
        assertXPath(pTable, "/x:pivotTableDefinition/x:pivotTableStyleInfo", "showColStripes", "0");
        assertXPath(pTable, "/x:pivotTableDefinition/x:pivotTableStyleInfo", "showLastColumn", "1");
    }
}

void ScPivotTableFiltersTest::testTdf124883()
{
    createScDoc("xlsx/pivot-table/two-data-fields.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pTable = parseExport("xl/pivotTables/pivotTable1.xml");
    CPPUNIT_ASSERT(pTable);

    // The field names must be kept just as they appear in original XLSX
    assertXPath(pTable, "/x:pivotTableDefinition/x:dataFields/x:dataField[1]", "name",
                "Sum of Value");
    assertXPath(pTable, "/x:pivotTableDefinition/x:dataFields/x:dataField[2]", "name",
                "Count of Value2");
}

void ScPivotTableFiltersTest::testTdf125046()
{
    createScDoc("xlsx/pivottable_long_text.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/pivotCache/pivotCacheDefinition1.xml");
    CPPUNIT_ASSERT(pDoc);
    assertXPath(pDoc, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[2]/x:sharedItems",
                "longText", "1");
}

void ScPivotTableFiltersTest::testTdf125055()
{
    createScDoc("xlsx/pivottable_1s_difference.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/pivotCache/pivotCacheDefinition1.xml");
    CPPUNIT_ASSERT(pDoc);

    // 1-second precision should not result in duplicated entries for values different by ~1 s.
    // Previously truncating nanoseconds in GetExcelFormattedDate converted
    // "2017-07-10T09:11:02.99999..." into "2017-07-10T09:11:02", creating two identical strings
    // Only compare times here: see comment to ScPivotTableFiltersTest::testPivotCacheExportXLSX
    // "TODO Date generator in tests are one day higher, than during standard xlsx export"
    OUString sISODateTime = getXPath(
        pDoc, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[2]/x:sharedItems", "minDate");
    CPPUNIT_ASSERT_EQUAL(OUString("T09:11:02"), sISODateTime.copy(10));
    sISODateTime = getXPath(
        pDoc, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[2]/x:sharedItems", "maxDate");
    CPPUNIT_ASSERT_EQUAL(OUString("T09:11:03"), sISODateTime.copy(10));
    assertXPath(pDoc, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[2]/x:sharedItems",
                "count", "3");
    assertXPathChildren(pDoc, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[2]/x:sharedItems",
                        3); // 2 different values + empty
    sISODateTime = getXPath(
        pDoc, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[2]/x:sharedItems/x:d[1]", "v");
    CPPUNIT_ASSERT_EQUAL(OUString("T09:11:02"), sISODateTime.copy(10));
    sISODateTime = getXPath(
        pDoc, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[2]/x:sharedItems/x:d[2]", "v");
    CPPUNIT_ASSERT_EQUAL(OUString("T09:11:03"), sISODateTime.copy(10));
    // Trailing empty
    CPPUNIT_ASSERT_EQUAL(
        2, getXPathPosition(
               pDoc, "/x:pivotCacheDefinition/x:cacheFields/x:cacheField[2]/x:sharedItems", "m"));
}

void ScPivotTableFiltersTest::testTdf125086()
{
    createScDoc("ods/pivottable_fieldInRowsAndData.ods");

    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/pivotTables/pivotTable1.xml");
    CPPUNIT_ASSERT(pDoc);
    assertXPath(pDoc, "/x:pivotTableDefinition/x:pivotFields/x:pivotField[2]", "axis", "axisRow");
    // "dataField" attribute was not written for this "axisRow" field
    assertXPath(pDoc, "/x:pivotTableDefinition/x:pivotFields/x:pivotField[2]", "dataField", "1");
}

void ScPivotTableFiltersTest::testTdf73845()
{
    // Query filter is set for individual pivot table in this ODS document.
    // This test checks the query filter is restored for ByEmpty and ByNonEmpty query.
    createScDoc("ods/pivottable_restore_query_filter.ods");
    ScDocument* pDoc = getScDoc();
    ScDPCollection* pDPs = pDoc->GetDPCollection();
    CPPUNIT_ASSERT_MESSAGE("Failed to get a live ScDPCollection instance.", pDPs);

    // Three pivot tables are created in the spreadsheet. Query filters are set as follow:
    // pivot table 0: Confirmed Date = Not Empty
    // pivot table 1: Confirmed Date = Empty
    // pivot table 2: Category > C1 AND Confirmed Date = Not Empty
    //
    // Following assertions check the Confirmed Date is restored properly
    // after file is opened again.
    const SCCOLROW nConfirmedDateCol = SCCOLROW(2);
    size_t nDPCount = pDPs->GetCount();
    for (size_t i = 0; i < nDPCount; i++)
    {
        ScDPObject& pDPObj = (*pDPs)[i];
        ScQueryParam aQueryParam(pDPObj.GetSheetDesc()->GetQueryParam());
        size_t nEntriesCount = aQueryParam.GetEntryCount();
        for (size_t j = 0; j < nEntriesCount; j++)
        {
            ScQueryEntry rEntry = aQueryParam.GetEntry(j);
            if (rEntry.IsQueryByEmpty() || rEntry.IsQueryByNonEmpty())
            {
                const OString msg = "Query entry " + OString::number(j) + " on pivot table "
                                    + OString::number(i) + " is not restored.";
                CPPUNIT_ASSERT_EQUAL_MESSAGE(msg.getStr(), nConfirmedDateCol, rEntry.nField);
            }
        }
    }
}

CPPUNIT_TEST_SUITE_REGISTRATION(ScPivotTableFiltersTest);

CPPUNIT_PLUGIN_IMPLEMENT();

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
