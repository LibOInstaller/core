/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <officecfg/Office/Common.hxx>
#include <config_fonts.h>

#include "helper/debughelper.hxx"

#include "helper/qahelper.hxx"
#include "helper/shared_test_impl.hxx"

#include <userdat.hxx>
#include <docpool.hxx>
#include <scitems.hxx>
#include <attrib.hxx>
#include <stlpool.hxx>
#include <editutil.hxx>
#include <scopetools.hxx>
#include <postit.hxx>
#include <tokenstringcontext.hxx>
#include <chgtrack.hxx>
#include <validat.hxx>
#include <scmod.hxx>

#include <svx/svdpage.hxx>
#include <svx/svdograf.hxx>
#include <svl/zformat.hxx>
#include <svl/numformat.hxx>
#include <tabprotection.hxx>
#include <editeng/wghtitem.hxx>
#include <editeng/postitem.hxx>
#include <editeng/eeitem.hxx>
#include <editeng/section.hxx>
#include <editeng/crossedoutitem.hxx>
#include <editeng/borderline.hxx>
#include <editeng/escapementitem.hxx>
#include <editeng/fontitem.hxx>
#include <editeng/fhgtitem.hxx>
#include <editeng/udlnitem.hxx>
#include <editeng/colritem.hxx>
#include <unotools/tempfile.hxx>
#include <unotools/useroptions.hxx>
#include <tools/datetime.hxx>

#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#include <com/sun/star/awt/XBitmap.hpp>
#include <com/sun/star/graphic/GraphicType.hpp>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

class ScExportTest : public ScModelTestBase
{
protected:
    virtual void registerNamespaces(xmlXPathContextPtr& pXmlXPathCtx) override;

public:
    ScExportTest()
        : ScModelTestBase("sc/qa/unit/data")
    {
    }

    void test();
    void testDefaultFontHeight();
    void testTdf139167();
    void testTdf113271();
    void testTdf139394();
    void testExtCondFormatXLSX();
    void testTdf90104();
    void testTdf111876();
    void testPasswordExport();
    void testTdf134332();
    void testConditionalFormatExportODS();
    void testConditionalFormatExportXLSX();
    void testCondFormatExportCellIs();
    void testTdf99856_dataValidationTest();
    void testProtectionKeyODS_UTF16LErtlSHA1();
    void testProtectionKeyODS_UTF8SHA1();
    void testProtectionKeyODS_UTF8SHA256ODF12();
    void testProtectionKeyODS_UTF8SHA256W3C();
    void testProtectionKeyODS_XL_SHA1();
    void testColorScaleExportODS();
    void testColorScaleExportXLSX();
    void testDataBarExportODS();
    void testDataBarExportXLSX();
    void testConditionalFormatRangeListXLSX();
    void testConditionalFormatContainsTextXLSX();
    void testConditionalFormatPriorityCheckXLSX();
    void testConditionalFormatOriginXLSX();
    void testTdf79998();
    void testLegacyCellAnchoredRotatedShape();
    void testMiscRowHeightExport();
    void testNamedRangeBugfdo62729();
    void testBuiltinRangesXLSX();
    void testRichTextExportODS();
    void testRichTextCellFormatXLSX();
    void testFormulaRefSheetNameODS();

    void testCellValuesExportODS();
    void testCellNoteExportODS();
    void testCellNoteExportXLS();
    void testFormatExportODS();

    void testCommentExportXLSX();
    void testCommentExportXLSX_2_XLSX();
#if HAVE_MORE_FONTS
    void testCustomColumnWidthExportXLSX();
#endif
    void testXfDefaultValuesXLSX();
    void testODF13();
    void testColumnWidthResaveXLSX();
#if HAVE_MORE_FONTS
    void testColumnWidthExportFromODStoXLSX();
#endif
    void testOutlineExportXLSX();
    void testHiddenEmptyRowsXLSX();
    void testHiddenEmptyColsODS();
    void testAllRowsHiddenXLSX();
    void testLandscapeOrientationXLSX();

    void testInlineArrayXLS();
    void testEmbeddedChartODS();
    void testEmbeddedChartXLS();
    void testCellAnchoredGroupXLS();

    void testFormulaReferenceXLS();
    void testSheetProtectionXLSX();
    void testSheetProtectionXLSB();

    void testCellBordersXLS();
    void testCellBordersXLSX();
    void testBordersExchangeXLSX();
    void testTrackChangesSimpleXLSX();
    void testSheetTabColorsXLSX();

    void testTdf133487();

    void testSharedFormulaExportXLS();
    void testSharedFormulaExportXLSX();
    void testSharedFormulaStringResultExportXLSX();

    void testFunctionsExcel2010(const OUString& sFormatType);
    void testFunctionsExcel2010XLSX();
    void testFunctionsExcel2010XLS();
    void testFunctionsExcel2010ODS();

    void testCeilingFloor(const OUString& sFormatType);
    void testCeilingFloorXLSX();
    void testCeilingFloorODSToXLSX();
    void testCeilingFloorXLS();
    void testCeilingFloorODS();

    void testCustomXml();

    void testRelativePathsODS();
    void testSheetProtectionODS();

    void testSwappedOutImageExport();
    void testLinkedGraphicRT();
    void testImageWithSpecialID();

    void testSupBookVirtualPathXLS();
    void testAbsNamedRangeHTML();
    void testTdf80149();
    void testSheetLocalRangeNameXLS();
    void testRelativeNamedExpressionsXLS();
    void testSheetTextBoxHyperlinkXLSX();
    void testFontSizeXLSX();
    void testSheetCharacterKerningSpaceXLSX();
    void testSheetCondensedCharacterSpaceXLSX();
    void testTextUnderlineColorXLSX();
    void testSheetRunParagraphPropertyXLSX();
    void testHiddenShapeXLS();
    void testHiddenShapeXLSX();
    void testShapeAutofitXLSX();
    void testHyperlinkXLSX();
    void testMoveCellAnchoredShapesODS();
    void testPreserveTextWhitespaceXLSX();
    void testPreserveTextWhitespace2XLSX();
    void testTdf113646();
    void testDateStandardfilterXLSX();
    void testNumberFormatODS();

    CPPUNIT_TEST_SUITE(ScExportTest);
    CPPUNIT_TEST(test);
    CPPUNIT_TEST(testDefaultFontHeight);
    CPPUNIT_TEST(testTdf139167);
    CPPUNIT_TEST(testTdf113271);
    CPPUNIT_TEST(testTdf139394);
    CPPUNIT_TEST(testExtCondFormatXLSX);
    CPPUNIT_TEST(testTdf90104);
    CPPUNIT_TEST(testTdf111876);
    CPPUNIT_TEST(testPasswordExport);
    CPPUNIT_TEST(testTdf134332);
    CPPUNIT_TEST(testConditionalFormatExportODS);
    CPPUNIT_TEST(testCondFormatExportCellIs);
    CPPUNIT_TEST(testConditionalFormatExportXLSX);
    CPPUNIT_TEST(testTdf99856_dataValidationTest);
    CPPUNIT_TEST(testProtectionKeyODS_UTF16LErtlSHA1);
    CPPUNIT_TEST(testProtectionKeyODS_UTF8SHA1);
    CPPUNIT_TEST(testProtectionKeyODS_UTF8SHA256ODF12);
    CPPUNIT_TEST(testProtectionKeyODS_UTF8SHA256W3C);
    CPPUNIT_TEST(testProtectionKeyODS_XL_SHA1);
    CPPUNIT_TEST(testColorScaleExportODS);
    CPPUNIT_TEST(testColorScaleExportXLSX);
    CPPUNIT_TEST(testDataBarExportODS);
    CPPUNIT_TEST(testDataBarExportXLSX);
    CPPUNIT_TEST(testConditionalFormatRangeListXLSX);
    CPPUNIT_TEST(testConditionalFormatContainsTextXLSX);
    CPPUNIT_TEST(testConditionalFormatPriorityCheckXLSX);
    CPPUNIT_TEST(testConditionalFormatOriginXLSX);
    CPPUNIT_TEST(testTdf79998);
    CPPUNIT_TEST(testLegacyCellAnchoredRotatedShape);
    CPPUNIT_TEST(testMiscRowHeightExport);
    CPPUNIT_TEST(testNamedRangeBugfdo62729);
    CPPUNIT_TEST(testBuiltinRangesXLSX);
    CPPUNIT_TEST(testRichTextExportODS);
    CPPUNIT_TEST(testRichTextCellFormatXLSX);
    CPPUNIT_TEST(testFormulaRefSheetNameODS);
    CPPUNIT_TEST(testCellValuesExportODS);
    CPPUNIT_TEST(testCellNoteExportODS);
    CPPUNIT_TEST(testCellNoteExportXLS);
    CPPUNIT_TEST(testFormatExportODS);
    CPPUNIT_TEST(testCommentExportXLSX);
    CPPUNIT_TEST(testCommentExportXLSX_2_XLSX);
#if HAVE_MORE_FONTS
    CPPUNIT_TEST(testCustomColumnWidthExportXLSX);
#endif
    CPPUNIT_TEST(testXfDefaultValuesXLSX);
    CPPUNIT_TEST(testODF13);
    CPPUNIT_TEST(testColumnWidthResaveXLSX);
#if HAVE_MORE_FONTS
    CPPUNIT_TEST(testColumnWidthExportFromODStoXLSX);
#endif
    CPPUNIT_TEST(testOutlineExportXLSX);
    CPPUNIT_TEST(testHiddenEmptyRowsXLSX);
    CPPUNIT_TEST(testHiddenEmptyColsODS);
    CPPUNIT_TEST(testAllRowsHiddenXLSX);
    CPPUNIT_TEST(testLandscapeOrientationXLSX);
    CPPUNIT_TEST(testInlineArrayXLS);
    CPPUNIT_TEST(testEmbeddedChartODS);
    CPPUNIT_TEST(testEmbeddedChartXLS);
    CPPUNIT_TEST(testCellAnchoredGroupXLS);

    CPPUNIT_TEST(testFormulaReferenceXLS);
    CPPUNIT_TEST(testSheetProtectionXLSX);
    CPPUNIT_TEST(testSheetProtectionXLSB);
    CPPUNIT_TEST(testCellBordersXLS);
    CPPUNIT_TEST(testCellBordersXLSX);
    CPPUNIT_TEST(testBordersExchangeXLSX);
    CPPUNIT_TEST(testTrackChangesSimpleXLSX);
    CPPUNIT_TEST(testSheetTabColorsXLSX);
    CPPUNIT_TEST(testTdf133487);
    CPPUNIT_TEST(testSharedFormulaExportXLS);
    CPPUNIT_TEST(testSharedFormulaExportXLSX);
    CPPUNIT_TEST(testSharedFormulaStringResultExportXLSX);
    CPPUNIT_TEST(testFunctionsExcel2010XLSX);
    CPPUNIT_TEST(testFunctionsExcel2010XLS);
    CPPUNIT_TEST(testFunctionsExcel2010ODS);
    CPPUNIT_TEST(testCeilingFloorXLSX);
    CPPUNIT_TEST(testCeilingFloorODSToXLSX);
    CPPUNIT_TEST(testCeilingFloorXLS);
    CPPUNIT_TEST(testCeilingFloorODS);
    CPPUNIT_TEST(testCustomXml);
    CPPUNIT_TEST(testRelativePathsODS);
    CPPUNIT_TEST(testSheetProtectionODS);
    CPPUNIT_TEST(testSupBookVirtualPathXLS);
    CPPUNIT_TEST(testSwappedOutImageExport);
    CPPUNIT_TEST(testLinkedGraphicRT);
    CPPUNIT_TEST(testImageWithSpecialID);
    CPPUNIT_TEST(testPreserveTextWhitespaceXLSX);
    CPPUNIT_TEST(testPreserveTextWhitespace2XLSX);
    CPPUNIT_TEST(testAbsNamedRangeHTML);
    CPPUNIT_TEST(testTdf80149);
    CPPUNIT_TEST(testSheetLocalRangeNameXLS);
    CPPUNIT_TEST(testRelativeNamedExpressionsXLS);
    CPPUNIT_TEST(testSheetTextBoxHyperlinkXLSX);
    CPPUNIT_TEST(testFontSizeXLSX);
    CPPUNIT_TEST(testSheetCharacterKerningSpaceXLSX);
    CPPUNIT_TEST(testSheetCondensedCharacterSpaceXLSX);
    CPPUNIT_TEST(testTextUnderlineColorXLSX);
    CPPUNIT_TEST(testSheetRunParagraphPropertyXLSX);
    CPPUNIT_TEST(testHiddenShapeXLS);
    CPPUNIT_TEST(testHiddenShapeXLSX);
    CPPUNIT_TEST(testShapeAutofitXLSX);
    CPPUNIT_TEST(testHyperlinkXLSX);
    CPPUNIT_TEST(testMoveCellAnchoredShapesODS);
    CPPUNIT_TEST(testTdf113646);
    CPPUNIT_TEST(testDateStandardfilterXLSX);
    CPPUNIT_TEST(testNumberFormatODS);
    CPPUNIT_TEST_SUITE_END();

private:
    void testExcelCellBorders(const OUString& sFormatType);
};

void ScExportTest::registerNamespaces(xmlXPathContextPtr& pXmlXPathCtx)
{
    XmlTestTools::registerOOXMLNamespaces(pXmlXPathCtx);
    XmlTestTools::registerODFNamespaces(pXmlXPathCtx);
}

void ScExportTest::test()
{
    createScDoc();

    ScDocument* pDoc = getScDoc();

    pDoc->SetValue(0, 0, 0, 1.0);

    saveAndReload("calc8");

    pDoc = getScDoc();
    double aVal = pDoc->GetValue(0, 0, 0);
    ASSERT_DOUBLES_EQUAL(aVal, 1.0);
}

void ScExportTest::testDefaultFontHeight()
{
    createScDoc();

    ScDocument* pDoc = getScDoc();
    ScDocumentPool* pPool = pDoc->GetPool();
    pPool->SetPoolDefaultItem(SvxFontHeightItem(400, 100, ATTR_FONT_HEIGHT));
    pPool->SetPoolDefaultItem(SvxFontHeightItem(400, 100, ATTR_CJK_FONT_HEIGHT));
    pPool->SetPoolDefaultItem(SvxFontHeightItem(400, 100, ATTR_CTL_FONT_HEIGHT));

    saveAndReload("calc8");

    pDoc = getScDoc();
    pPool = pDoc->GetPool();
    const SvxFontHeightItem& rItem = pPool->GetDefaultItem(ATTR_FONT_HEIGHT);
    CPPUNIT_ASSERT_EQUAL(sal_uInt32(400), rItem.GetHeight());
    const SvxFontHeightItem& rCJKItem = pPool->GetDefaultItem(ATTR_CJK_FONT_HEIGHT);
    CPPUNIT_ASSERT_EQUAL(sal_uInt32(400), rCJKItem.GetHeight());
    const SvxFontHeightItem& rCTLItem = pPool->GetDefaultItem(ATTR_CTL_FONT_HEIGHT);
    CPPUNIT_ASSERT_EQUAL(sal_uInt32(400), rCTLItem.GetHeight());
}

void ScExportTest::testTdf139167()
{
    createScDoc("xlsx/tdf139167.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/styles.xml");
    CPPUNIT_ASSERT(pDoc);

    assertXPath(pDoc, "/x:styleSheet/x:cellStyles", "count", "6");
    assertXPath(pDoc, "/x:styleSheet/x:dxfs/x:dxf/x:fill/x:patternFill/x:bgColor", "rgb",
                "FFFFFF00");
}

void ScExportTest::testTdf113271()
{
    createScDoc("xlsx/tdf113271.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/styles.xml");
    CPPUNIT_ASSERT(pDoc);

    assertXPath(pDoc, "/x:styleSheet/x:fonts", "count", "6");

    // Without the fix in place, this test would have failed with
    // - Expected: FF000000
    // - Actual  : FFFFFFFF
    assertXPath(pDoc, "/x:styleSheet/x:fonts/x:font[1]/x:color", "rgb", "FF000000");
    assertXPath(pDoc, "/x:styleSheet/x:fonts/x:font[1]/x:name", "val", "Calibri");
}

void ScExportTest::testTdf139394()
{
    createScDoc("xlsx/tdf139394.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/worksheets/sheet1.xml");
    CPPUNIT_ASSERT(pDoc);

    assertXPathContent(
        pDoc,
        "/x:worksheet/x:extLst/x:ext/x14:conditionalFormattings/x14:conditionalFormatting[1]/"
        "x14:cfRule/xm:f",
        "LEFT(A1,LEN(\"+\"))=\"+\"");
    assertXPathContent(
        pDoc,
        "/x:worksheet/x:extLst/x:ext/x14:conditionalFormattings/x14:conditionalFormatting[2]/"
        "x14:cfRule/xm:f",
        "RIGHT(A2,LEN(\"-\"))=\"-\"");
    assertXPathContent(
        pDoc,
        "/x:worksheet/x:extLst/x:ext/x14:conditionalFormattings/x14:conditionalFormatting[3]/"
        "x14:cfRule/xm:f",
        "LEFT(A3,LEN($B$3))=$B$3");
}

void ScExportTest::testExtCondFormatXLSX()
{
    createScDoc("xlsx/tdf139021.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/worksheets/sheet1.xml");
    CPPUNIT_ASSERT(pDoc);

    assertXPath(
        pDoc,
        "/x:worksheet/x:extLst/x:ext/x14:conditionalFormattings/x14:conditionalFormatting[1]/"
        "x14:cfRule",
        "type", "containsText");
    assertXPathContent(
        pDoc,
        "/x:worksheet/x:extLst/x:ext/x14:conditionalFormattings/x14:conditionalFormatting[1]/"
        "x14:cfRule/xm:f[1]",
        "NOT(ISERROR(SEARCH($B$1,A1)))");
    assertXPathContent(
        pDoc,
        "/x:worksheet/x:extLst/x:ext/x14:conditionalFormattings/x14:conditionalFormatting[1]/"
        "x14:cfRule/xm:f[2]",
        "$B$1");
    assertXPath(
        pDoc,
        "/x:worksheet/x:extLst/x:ext/x14:conditionalFormattings/x14:conditionalFormatting[2]/"
        "x14:cfRule",
        "type", "notContainsText");
    assertXPathContent(
        pDoc,
        "/x:worksheet/x:extLst/x:ext/x14:conditionalFormattings/x14:conditionalFormatting[2]/"
        "x14:cfRule/xm:f[1]",
        "ISERROR(SEARCH($B$2,A2))");
    assertXPathContent(
        pDoc,
        "/x:worksheet/x:extLst/x:ext/x14:conditionalFormattings/x14:conditionalFormatting[2]/"
        "x14:cfRule/xm:f[2]",
        "$B$2");
}

void ScExportTest::testTdf90104()
{
    createScDoc("xlsx/tdf90104.xlsx");

    save("Calc Office Open XML");

    xmlDocUniquePtr pDoc = parseExport("xl/worksheets/sheet1.xml");
    CPPUNIT_ASSERT(pDoc);

    assertXPathContent(pDoc,
                       "/x:worksheet/x:dataValidations/x:dataValidation/mc:AlternateContent"
                       "/mc:Choice/x12ac:list",
                       "1,\"2,3\",4,\"5,6\"");
    assertXPathContent(pDoc,
                       "/x:worksheet/x:dataValidations/x:dataValidation/mc:AlternateContent"
                       "/mc:Fallback/x:formula1",
                       "\"1,2,3,4,5,6\"");
}

void ScExportTest::testTdf111876()
{
    // Document with relative path hyperlink

    createScDoc("xlsx/tdf111876.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/worksheets/_rels/sheet1.xml.rels");
    CPPUNIT_ASSERT(pDoc);
    OUString sTarget = getXPath(pDoc, "/rels:Relationships/rels:Relationship", "Target");

    // Document is saved to the temporary directory, relative path should be different than original one
    CPPUNIT_ASSERT(sTarget != "../xls/bug-fixes.xls");
}

void ScExportTest::testPasswordExport()
{
    std::vector<OUString> aFilterNames{ "calc8", "MS Excel 97", "Calc Office Open XML" };

    for (size_t i = 0; i < aFilterNames.size(); ++i)
    {
        createScDoc();

        ScDocument* pDoc = getScDoc();

        pDoc->SetValue(0, 0, 0, 1.0);

        saveAndReload(aFilterNames[i], /*pPassword*/ "test");

        pDoc = getScDoc();
        double aVal = pDoc->GetValue(0, 0, 0);
        ASSERT_DOUBLES_EQUAL(aVal, 1.0);
    }
}

void ScExportTest::testTdf134332()
{
    createScDoc("ods/tdf134332.ods");

    ScDocument* pDoc = getScDoc();

    ASSERT_DOUBLES_EQUAL(190.0, pDoc->GetValue(ScAddress(0, 0, 0)));

    ASSERT_DOUBLES_EQUAL(238.0, pDoc->GetValue(ScAddress(0, 10144, 0)));

    saveAndReload("calc8", /*pPassword*/ "test");

    // Without the fixes in place, it would have failed here
    pDoc = getScDoc();
    ASSERT_DOUBLES_EQUAL(190.0, pDoc->GetValue(ScAddress(0, 0, 0)));

    ASSERT_DOUBLES_EQUAL(238.0, pDoc->GetValue(ScAddress(0, 10144, 0)));
}

void ScExportTest::testConditionalFormatExportODS()
{
    createScDoc("ods/new_cond_format_test_export.ods");

    saveAndReload("calc8");
    ScDocument* pDoc = getScDoc();
    OUString aCSVPath = createFilePath(u"contentCSV/new_cond_format_test_export.csv");
    testCondFile(aCSVPath, &*pDoc, 0);
}

void ScExportTest::testCondFormatExportCellIs()
{
    createScDoc("xlsx/condFormat_cellis.xlsx");
    saveAndReload("Calc Office Open XML");

    ScDocument* pDoc = getScDoc();
    CPPUNIT_ASSERT_EQUAL(size_t(1), pDoc->GetCondFormList(0)->size());

    ScConditionalFormat* pFormat = pDoc->GetCondFormat(0, 0, 0);
    CPPUNIT_ASSERT(pFormat);

    const ScFormatEntry* pEntry = pFormat->GetEntry(0);
    CPPUNIT_ASSERT(pEntry);
    CPPUNIT_ASSERT_EQUAL(ScFormatEntry::Type::ExtCondition, pEntry->GetType());

    const ScCondFormatEntry* pCondition = static_cast<const ScCondFormatEntry*>(pEntry);
    CPPUNIT_ASSERT_EQUAL(ScConditionMode::Equal, pCondition->GetOperation());

    OUString aStr = pCondition->GetExpression(ScAddress(0, 0, 0), 0);
    CPPUNIT_ASSERT_EQUAL(OUString("$Sheet2.$A$2"), aStr);

    pEntry = pFormat->GetEntry(1);
    CPPUNIT_ASSERT(pEntry);
    CPPUNIT_ASSERT_EQUAL(ScFormatEntry::Type::ExtCondition, pEntry->GetType());

    pCondition = static_cast<const ScCondFormatEntry*>(pEntry);
    CPPUNIT_ASSERT_EQUAL(ScConditionMode::Equal, pCondition->GetOperation());

    aStr = pCondition->GetExpression(ScAddress(0, 0, 0), 0);
    CPPUNIT_ASSERT_EQUAL(OUString("$Sheet2.$A$1"), aStr);
}

void ScExportTest::testConditionalFormatExportXLSX()
{
    createScDoc("xlsx/new_cond_format_test_export.xlsx");

    saveAndReload("Calc Office Open XML");
    ScDocument* pDoc = getScDoc();
    {
        OUString aCSVPath = createFilePath(u"contentCSV/new_cond_format_test_export.csv");
        testCondFile(aCSVPath, &*pDoc, 0);
    }
    {
        OUString aCSVPath = createFilePath(u"contentCSV/new_cond_format_test_sheet2.csv");
        testCondFile(aCSVPath, &*pDoc, 1);
    }
}

void ScExportTest::testTdf99856_dataValidationTest()
{
    createScDoc("ods/tdf99856_dataValidationTest.ods");

    saveAndReload("Calc Office Open XML");

    ScDocument* pDoc = getScDoc();
    const ScValidationData* pData = pDoc->GetValidationEntry(2);
    CPPUNIT_ASSERT(pData);

    // Excel can't open corrupt file if the list is longer than 255 characters
    std::vector<ScTypedStrData> aList;
    pData->FillSelectionList(aList, ScAddress(0, 1, 1));
    CPPUNIT_ASSERT_EQUAL(size_t(18), aList.size());
    CPPUNIT_ASSERT_EQUAL(OUString("18 Missis"), aList[17].GetString());
}

void ScExportTest::testProtectionKeyODS_UTF16LErtlSHA1()
{
    OUString const password("1012345678901234567890123456789012345678901234567890");

    createScDoc("fods/protection-key1.fods");

    ScDocument* pDoc = getScDoc();
    ScDocProtection* const pDocProt(pDoc->GetDocProtection());
    CPPUNIT_ASSERT(pDocProt->verifyPassword(password));
    const ScTableProtection* const pTabProt(pDoc->GetTabProtection(0));
    CPPUNIT_ASSERT(pTabProt->verifyPassword(password));

    // we can't assume that the user entered the password; check that we
    // round-trip the password as-is
    save("calc8");
    xmlDocUniquePtr pXmlDoc = parseExport("content.xml");
    assertXPath(pXmlDoc,
                "//office:spreadsheet[@table:structure-protected='true' and "
                "@table:protection-key='vbnhxyBKtPHCA1wB21zG1Oha8ZA=' and "
                "@table:protection-key-digest-algorithm='http://www.w3.org/2000/09/xmldsig#sha1']");
    assertXPath(pXmlDoc,
                "//table:table[@table:protected='true' and "
                "@table:protection-key='vbnhxyBKtPHCA1wB21zG1Oha8ZA=' and "
                "@table:protection-key-digest-algorithm='http://www.w3.org/2000/09/xmldsig#sha1']");
}

void ScExportTest::testProtectionKeyODS_UTF8SHA1()
{
    OUString const password("1012345678901234567890123456789012345678901234567890");

    createScDoc("fods/protection-key2.fods");

    ScDocument* pDoc = getScDoc();
    ScDocProtection* const pDocProt(pDoc->GetDocProtection());
    CPPUNIT_ASSERT(pDocProt->verifyPassword(password));
    const ScTableProtection* const pTabProt(pDoc->GetTabProtection(0));
    CPPUNIT_ASSERT(pTabProt->verifyPassword(password));

    // we can't assume that the user entered the password; check that we
    // round-trip the password as-is
    save("calc8");
    xmlDocUniquePtr pXmlDoc = parseExport("content.xml");
    assertXPath(pXmlDoc,
                "//office:spreadsheet[@table:structure-protected='true' and "
                "@table:protection-key='nLHas0RIwepGDaH4c2hpyIUvIS8=' and "
                "@table:protection-key-digest-algorithm='http://www.w3.org/2000/09/xmldsig#sha1']");
    assertXPath(pXmlDoc,
                "//table:table[@table:protected='true' and "
                "@table:protection-key='nLHas0RIwepGDaH4c2hpyIUvIS8=' and "
                "@table:protection-key-digest-algorithm='http://www.w3.org/2000/09/xmldsig#sha1']");
}

void ScExportTest::testProtectionKeyODS_UTF8SHA256ODF12()
{
    OUString const password("1012345678901234567890123456789012345678901234567890");

    createScDoc("fods/protection-key3.fods");

    ScDocument* pDoc = getScDoc();
    ScDocProtection* const pDocProt(pDoc->GetDocProtection());
    CPPUNIT_ASSERT(pDocProt->verifyPassword(password));
    const ScTableProtection* const pTabProt(pDoc->GetTabProtection(0));
    CPPUNIT_ASSERT(pTabProt->verifyPassword(password));

    // we can't assume that the user entered the password; check that we
    // round-trip the password as-is
    save("calc8");
    xmlDocUniquePtr pXmlDoc = parseExport("content.xml");
    assertXPath(
        pXmlDoc,
        "//office:spreadsheet[@table:structure-protected='true' and "
        "@table:protection-key='1tnJohagR2T0yF/v69hLPuumSTsj32CumW97nkKGuSQ=' and "
        "@table:protection-key-digest-algorithm='http://www.w3.org/2000/09/xmldsig#sha256']");
    assertXPath(
        pXmlDoc,
        "//table:table[@table:protected='true' and "
        "@table:protection-key='1tnJohagR2T0yF/v69hLPuumSTsj32CumW97nkKGuSQ=' and "
        "@table:protection-key-digest-algorithm='http://www.w3.org/2000/09/xmldsig#sha256']");
}

void ScExportTest::testProtectionKeyODS_UTF8SHA256W3C()
{
    OUString const password("1012345678901234567890123456789012345678901234567890");

    createScDoc("fods/protection-key4.fods");

    ScDocument* pDoc = getScDoc();
    ScDocProtection* const pDocProt(pDoc->GetDocProtection());
    CPPUNIT_ASSERT(pDocProt->verifyPassword(password));
    const ScTableProtection* const pTabProt(pDoc->GetTabProtection(0));
    CPPUNIT_ASSERT(pTabProt->verifyPassword(password));

    // we can't assume that the user entered the password; check that we
    // round-trip the password as-is
    save("calc8");
    xmlDocUniquePtr pXmlDoc = parseExport("content.xml");
    assertXPath(
        pXmlDoc,
        "//office:spreadsheet[@table:structure-protected='true' and "
        "@table:protection-key='1tnJohagR2T0yF/v69hLPuumSTsj32CumW97nkKGuSQ=' and "
        "@table:protection-key-digest-algorithm='http://www.w3.org/2000/09/xmldsig#sha256']");
    assertXPath(
        pXmlDoc,
        "//table:table[@table:protected='true' and "
        "@table:protection-key='1tnJohagR2T0yF/v69hLPuumSTsj32CumW97nkKGuSQ=' and "
        "@table:protection-key-digest-algorithm='http://www.w3.org/2000/09/xmldsig#sha256']");
}

void ScExportTest::testProtectionKeyODS_XL_SHA1()
{
    OUString const password("1012345678901234567890123456789012345678901234567890");

    createScDoc("fods/protection-key5.fods");

    ScDocument* pDoc = getScDoc();
    ScDocProtection* const pDocProt(pDoc->GetDocProtection());
    CPPUNIT_ASSERT(pDocProt->verifyPassword(password));
    const ScTableProtection* const pTabProt(pDoc->GetTabProtection(0));
    CPPUNIT_ASSERT(pTabProt->verifyPassword(password));

    // we can't assume that the user entered the password; check that we
    // round-trip the password as-is
    save("calc8");
    xmlDocUniquePtr pXmlDoc = parseExport("content.xml");
    assertXPath(
        pXmlDoc,
        "//office:spreadsheet[@table:structure-protected='true' and "
        "@table:protection-key='OX3WkEe79fv1PE+FUmfOLdwVoqI=' and "
        "@table:protection-key-digest-algorithm='http://docs.oasis-open.org/office/ns/table/"
        "legacy-hash-excel' and "
        "@loext:protection-key-digest-algorithm-2='http://www.w3.org/2000/09/xmldsig#sha1']");
    assertXPath(
        pXmlDoc,
        "//table:table[@table:protected='true' and "
        "@table:protection-key='OX3WkEe79fv1PE+FUmfOLdwVoqI=' and "
        "@table:protection-key-digest-algorithm='http://docs.oasis-open.org/office/ns/table/"
        "legacy-hash-excel' and "
        "@loext:protection-key-digest-algorithm-2='http://www.w3.org/2000/09/xmldsig#sha1']");
}

void ScExportTest::testColorScaleExportODS()
{
    createScDoc("ods/colorscale.ods");

    saveAndReload("calc8");

    ScDocument* pDoc = getScDoc();

    testColorScale2Entry_Impl(*pDoc);
    testColorScale3Entry_Impl(*pDoc);
}

void ScExportTest::testColorScaleExportXLSX()
{
    createScDoc("xlsx/colorscale.xlsx");

    saveAndReload("Calc Office Open XML");

    ScDocument* pDoc = getScDoc();

    testColorScale2Entry_Impl(*pDoc);
    testColorScale3Entry_Impl(*pDoc);
}

void ScExportTest::testDataBarExportODS()
{
    createScDoc("ods/databar.ods");

    saveAndReload("calc8");

    ScDocument* pDoc = getScDoc();

    testDataBar_Impl(*pDoc);
}

void ScExportTest::testFormatExportODS()
{
    createScDoc("ods/formats.ods");

    saveAndReload("calc8");

    ScDocument* pDoc = getScDoc();

    testFormats(pDoc, u"calc8");
}

void ScExportTest::testCommentExportXLSX()
{
    //tdf#104729 FILESAVE OpenOffice do not save author of the comment during export to .xlsx
    createScDoc("ods/comment.ods");

    save("Calc Office Open XML");
    xmlDocUniquePtr pComments = parseExport("xl/comments1.xml");
    CPPUNIT_ASSERT(pComments);

    assertXPathContent(pComments, "/x:comments/x:authors/x:author[1]", "BAKO");
    assertXPath(pComments, "/x:comments/x:authors/x:author", 1);

    assertXPathContent(pComments, "/x:comments/x:commentList/x:comment/x:text/x:r/x:t",
                       "Komentarz");

    xmlDocUniquePtr pVmlDrawing = parseExport("xl/drawings/vmlDrawing1.vml");
    CPPUNIT_ASSERT(pVmlDrawing);

    //assertXPath(pVmlDrawing, "/xml/v:shapetype", "coordsize", "21600,21600");
    assertXPath(pVmlDrawing, "/xml/v:shapetype", "spt", "202");
    assertXPath(pVmlDrawing, "/xml/v:shapetype/v:stroke", "joinstyle", "miter");
    const OUString sShapeTypeId = "#" + getXPath(pVmlDrawing, "/xml/v:shapetype", "id");

    assertXPath(pVmlDrawing, "/xml/v:shape", "type", sShapeTypeId);
    assertXPath(pVmlDrawing, "/xml/v:shape/v:shadow", "color", "black");
    assertXPath(pVmlDrawing, "/xml/v:shape/v:shadow", "obscured", "t");

    //tdf#117274 fix MSO interoperability with the secret VML shape type id
    assertXPath(pVmlDrawing, "/xml/v:shapetype", "id", "_x0000_t202");
    assertXPath(pVmlDrawing, "/xml/v:shape", "type", "#_x0000_t202");
}

void ScExportTest::testCommentExportXLSX_2_XLSX()
{
    //tdf#117287 FILESAVE XLSX: Comments always disappear after opening the exported XLSX file with Excel
    createScDoc("xlsx/tdf117287_comment.xlsx");

    ScDocument* pDoc = getScDoc();
    ScAddress aPosC9(2, 8, 0);
    ScPostIt* pNote = pDoc->GetNote(aPosC9);

    CPPUNIT_ASSERT(pNote);
    CPPUNIT_ASSERT(!pNote->IsCaptionShown());

    pNote->ShowCaption(aPosC9, true);

    save("Calc Office Open XML");
    xmlDocUniquePtr pComments = parseExport("xl/comments1.xml");
    CPPUNIT_ASSERT(pComments);

    assertXPathContent(pComments, "/x:comments/x:commentList/x:comment/x:text/x:r/x:t",
                       "visible comment");

    xmlDocUniquePtr pVmlDrawing = parseExport("xl/drawings/vmlDrawing1.vml");
    CPPUNIT_ASSERT(pVmlDrawing);

    assertXPath(pVmlDrawing, "/xml/v:shape/x:ClientData/x:Visible", 0);
}

#if HAVE_MORE_FONTS
void ScExportTest::testCustomColumnWidthExportXLSX()
{
    //tdf#100946 FILESAVE Excel on macOS ignored column widths in XLSX last saved by LO
    createScDoc("ods/custom_column_width.ods");

    save("Calc Office Open XML");
    xmlDocUniquePtr pSheet = parseExport("xl/worksheets/sheet1.xml");
    CPPUNIT_ASSERT(pSheet);

    // tdf#124741: check that we export default width, otherwise the skipped columns would have
    // wrong width. Previously defaultColWidth attribute was missing
    double nDefWidth
        = getXPath(pSheet, "/x:worksheet/x:sheetFormatPr", "defaultColWidth").toDouble();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(11.53515625, nDefWidth, 0.01);

    // First column, has everything default (width in Calc: 1280), skipped

    // Second column, has custom width (width in Calc: 1225)
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[1]", "hidden", "false");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[1]", "outlineLevel", "0");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[1]", "customWidth", "true");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[1]", "collapsed", "false");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[1]", "min", "2");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[1]", "max", "2");

    // Third column, has everything default (width in Calc: 1280), skipped

    // Fourth column has custom width. Columns from 4 to 7 are hidden
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[2]", "hidden", "true");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[2]", "outlineLevel", "0");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[2]", "customWidth", "true");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[2]", "collapsed", "false");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[2]", "min", "4");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[2]", "max", "4");

    // 5th column has custom width. Columns from 4 to 7 are hidden
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[3]", "hidden", "true");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[3]", "outlineLevel", "0");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[3]", "customWidth", "true");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[3]", "collapsed", "false");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[3]", "min", "5");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[3]", "max", "5");

    // 6th and 7th columns have default width and they are hidden
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[4]", "hidden", "true");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[4]", "outlineLevel", "0");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[4]", "customWidth", "false");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[4]", "collapsed", "false");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[4]", "min", "6");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[4]", "max", "7");

    // 8th column has everything default - skipped

    // 9th column has custom width
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[5]", "hidden", "false");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[5]", "outlineLevel", "0");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[5]", "customWidth", "true");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[5]", "collapsed", "false");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[5]", "min", "9");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[5]", "max", "9");

    // We expected that exactly 5 unique Nodes will be produced
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col", 5);

    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[1]", "hidden", "false");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[1]", "outlineLevel", "0");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[1]", "collapsed", "false");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[1]", "customFormat", "false");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[1]", "customHeight", "false");
}
#endif

void ScExportTest::testXfDefaultValuesXLSX()
{
    //tdf#70565 FORMATTING: User Defined Custom Formatting is not applied during importing XLSX documents
    createScDoc("xlsx/xf_default_values.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pSheet = parseExport("xl/styles.xml");
    CPPUNIT_ASSERT(pSheet);

    // cellStyleXfs don't need xfId, so we need to make sure it is not saved
    assertXPathNoAttribute(pSheet, "/x:styleSheet/x:cellStyleXfs/x:xf[1]", "xfId");

    // Because numFmtId fontId fillId borderId xfId are not existing during import
    // it should be created during export, with values set to "0"
    assertXPath(pSheet, "/x:styleSheet/x:cellXfs/x:xf[1]", "xfId", "0");
    assertXPath(pSheet, "/x:styleSheet/x:cellXfs/x:xf[2]", "xfId", "0");
    assertXPath(pSheet, "/x:styleSheet/x:cellXfs/x:xf[3]", "xfId", "0");
    assertXPath(pSheet, "/x:styleSheet/x:cellXfs/x:xf[4]", "xfId", "0");

    // We expected that exactly 15 cellXfs:xf Nodes will be produced
    assertXPath(pSheet, "/x:styleSheet/x:cellXfs/x:xf", 14);
}

static auto verifySpreadsheet13(char const* const pTestName, ScDocument& rDoc) -> void
{
    // OFFICE-2173 table:tab-color
    CPPUNIT_ASSERT_EQUAL_MESSAGE(pTestName, Color(0xff3838), rDoc.GetTabBgColor(0));
    // OFFICE-3857 table:scale-to-X/table:scale-to-Y
    OUString styleName = rDoc.GetPageStyle(0);
    ScStyleSheetPool* pStylePool = rDoc.GetStyleSheetPool();
    SfxStyleSheetBase* pStyleSheet = pStylePool->Find(styleName, SfxStyleFamily::Page);
    CPPUNIT_ASSERT_MESSAGE(pTestName, pStyleSheet);

    SfxItemSet const& rSet = pStyleSheet->GetItemSet();
    ScPageScaleToItem const& rItem(rSet.Get(ATTR_PAGE_SCALETO));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(pTestName, sal_uInt16(2), rItem.GetWidth());
    CPPUNIT_ASSERT_EQUAL_MESSAGE(pTestName, sal_uInt16(3), rItem.GetHeight());
}

void ScExportTest::testODF13()
{
    // import
    createScDoc("ods/spreadsheet13e.ods");
    ScDocument* pDoc = getScDoc();

    // check model
    verifySpreadsheet13("import", *pDoc);

    Resetter _([]() {
        std::shared_ptr<comphelper::ConfigurationChanges> pBatch(
            comphelper::ConfigurationChanges::create());
        officecfg::Office::Common::Save::ODF::DefaultVersion::set(3, pBatch);
        return pBatch->commit();
    });

    {
        // export ODF 1.3
        std::shared_ptr<comphelper::ConfigurationChanges> pBatch(
            comphelper::ConfigurationChanges::create());
        officecfg::Office::Common::Save::ODF::DefaultVersion::set(10, pBatch);
        pBatch->commit();

        // FIXME: Error: unexpected attribute "loext:scale-to-X"
        skipValidation();

        saveAndReload("calc8");
        pDoc = getScDoc();

        // check XML
        xmlDocUniquePtr pContentXml = parseExport("content.xml");
        assertXPath(pContentXml, "/office:document-content/office:automatic-styles/style:style/"
                                 "style:table-properties[@table:tab-color='#ff3838']");
        xmlDocUniquePtr pStylesXml = parseExport("styles.xml");
        assertXPath(pStylesXml, "/office:document-styles/office:automatic-styles/style:page-layout/"
                                "style:page-layout-properties[@style:scale-to-X='2']");
        assertXPath(pStylesXml, "/office:document-styles/office:automatic-styles/style:page-layout/"
                                "style:page-layout-properties[@style:scale-to-Y='3']");

        // check model
        verifySpreadsheet13("1.3 reload", *pDoc);
    }
    {
        // export ODF 1.2 Extended
        std::shared_ptr<comphelper::ConfigurationChanges> pBatch(
            comphelper::ConfigurationChanges::create());
        officecfg::Office::Common::Save::ODF::DefaultVersion::set(9, pBatch);
        pBatch->commit();

        saveAndReload("calc8");
        pDoc = getScDoc();

        // check XML
        xmlDocUniquePtr pContentXml = parseExport("content.xml");
        assertXPath(pContentXml, "/office:document-content/office:automatic-styles/style:style/"
                                 "style:table-properties[@tableooo:tab-color='#ff3838']");
        xmlDocUniquePtr pStylesXml = parseExport("styles.xml");
        assertXPath(pStylesXml, "/office:document-styles/office:automatic-styles/style:page-layout/"
                                "style:page-layout-properties[@loext:scale-to-X='2']");
        assertXPath(pStylesXml, "/office:document-styles/office:automatic-styles/style:page-layout/"
                                "style:page-layout-properties[@loext:scale-to-Y='3']");

        // check model
        verifySpreadsheet13("1.2 Extended reload", *pDoc);
    }
    {
        // export ODF 1.2
        std::shared_ptr<comphelper::ConfigurationChanges> pBatch(
            comphelper::ConfigurationChanges::create());
        officecfg::Office::Common::Save::ODF::DefaultVersion::set(4, pBatch);
        pBatch->commit();

        save("calc8");

        // check XML
        xmlDocUniquePtr pContentXml = parseExport("content.xml");
        assertXPathNoAttribute(
            pContentXml,
            "/office:document-content/office:automatic-styles/style:style/style:table-properties",
            "tab-color");
        xmlDocUniquePtr pStylesXml = parseExport("styles.xml");
        assertXPathNoAttribute(pStylesXml,
                               "/office:document-styles/office:automatic-styles/"
                               "style:page-layout[1]/style:page-layout-properties",
                               "scale-to-X");
        assertXPathNoAttribute(pStylesXml,
                               "/office:document-styles/office:automatic-styles/"
                               "style:page-layout[1]/style:page-layout-properties",
                               "scale-to-Y");

        // don't reload - no point
    }
}

void ScExportTest::testColumnWidthResaveXLSX()
{
    // tdf#91475 FILESAVE: Column width is not preserved in XLSX / after round trip.
    // Test if after resave .xlsx file, columns width is identical with previous one
    createScDoc("xlsx/different-column-width-excel2010.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pSheet = parseExport("xl/worksheets/sheet1.xml");
    CPPUNIT_ASSERT(pSheet);

    // In original Excel document the width is "24"
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[1]", "width", "24");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[1]", "customWidth", "true");

    // In original Excel document the width is "12"
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[2]", "width", "12");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[2]", "customWidth", "true");

    // In original Excel document the width is "6"
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[3]", "width", "6");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[3]", "customWidth", "true");

    // In original Excel document the width is "1"
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[4]", "width", "1");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[4]", "customWidth", "true");

    // In original Excel document the width is "250"
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[5]", "width", "250");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[5]", "customWidth", "true");

    assertXPath(pSheet, "/x:worksheet/x:cols/x:col", 5);
}

#if HAVE_MORE_FONTS
void ScExportTest::testColumnWidthExportFromODStoXLSX()
{
    // tdf#91475 FILESAVE: Column width is not preserved in XLSX / after round trip.
    // Test if after export .ods to .xlsx format, displayed columns width
    // is identical with previous (.ods) one

    createScDoc("ods/different-column-width.ods");

    ScDocument* pDoc = getScDoc();

    // Col 1, Tab 0 (Column width 2.00 in)
    sal_uInt16 nExpectedColumn0Width
        = pDoc->GetColWidth(static_cast<SCCOL>(0), static_cast<SCTAB>(0), false);
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_uInt16>(2880), nExpectedColumn0Width);

    // Col 2, Tab 0 (Column width 1.00 in)
    sal_uInt16 nExpectedColumn1Width
        = pDoc->GetColWidth(static_cast<SCCOL>(1), static_cast<SCTAB>(0), false);
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_uInt16>(1440), nExpectedColumn1Width);

    // Col 3, Tab 0 (Column width 0.50 in)
    sal_uInt16 nExpectedColumn2Width
        = pDoc->GetColWidth(static_cast<SCCOL>(2), static_cast<SCTAB>(0), false);
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_uInt16>(720), nExpectedColumn2Width);

    // Col 4, Tab 0 (Column width 0.25 in)
    sal_uInt16 nExpectedColumn3Width
        = pDoc->GetColWidth(static_cast<SCCOL>(3), static_cast<SCTAB>(0), false);
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_uInt16>(360), nExpectedColumn3Width);

    // Col 5, Tab 0 (Column width 13.57 in)
    sal_uInt16 nExpectedColumn4Width
        = pDoc->GetColWidth(static_cast<SCCOL>(4), static_cast<SCTAB>(0), false);
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_uInt16>(19539), nExpectedColumn4Width);

    // Export to .xlsx and compare column width with the .ods
    // We expect that column width from .ods will be exactly the same as imported from .xlsx

    saveAndReload("Calc Office Open XML");

    pDoc = getScDoc();

    // Col 1, Tab 0
    sal_uInt16 nCalcWidth;
    nCalcWidth = pDoc->GetColWidth(static_cast<SCCOL>(0), static_cast<SCTAB>(0), false);
    CPPUNIT_ASSERT_EQUAL(nExpectedColumn0Width, nCalcWidth);

    // Col 2, Tab 0
    nCalcWidth = pDoc->GetColWidth(static_cast<SCCOL>(1), static_cast<SCTAB>(0), false);
    CPPUNIT_ASSERT_EQUAL(nExpectedColumn1Width, nCalcWidth);

    // Col 3, Tab 0
    nCalcWidth = pDoc->GetColWidth(static_cast<SCCOL>(2), static_cast<SCTAB>(0), false);
    CPPUNIT_ASSERT_EQUAL(nExpectedColumn2Width, nCalcWidth);

    // Col 4, Tab 0
    nCalcWidth = pDoc->GetColWidth(static_cast<SCCOL>(3), static_cast<SCTAB>(0), false);
    CPPUNIT_ASSERT_EQUAL(nExpectedColumn3Width, nCalcWidth);

    // Col 5, Tab 0
    nCalcWidth = pDoc->GetColWidth(static_cast<SCCOL>(4), static_cast<SCTAB>(0), false);
    CPPUNIT_ASSERT_EQUAL(nExpectedColumn4Width, nCalcWidth);
}
#endif

void ScExportTest::testOutlineExportXLSX()
{
    //tdf#100347 FILESAVE FILEOPEN after exporting to .xlsx format grouping are lost
    //tdf#51524  FILESAVE .xlsx and.xls looses width information for hidden/collapsed grouped columns
    createScDoc("ods/outline.ods");

    save("Calc Office Open XML");
    xmlDocUniquePtr pSheet = parseExport("xl/worksheets/sheet1.xml");
    CPPUNIT_ASSERT(pSheet);

    // Maximum Outline Row is 4 for this document
    assertXPath(pSheet, "/x:worksheet/x:sheetFormatPr", "outlineLevelRow", "4");
    // Maximum Outline Column is 4 for this document
    assertXPath(pSheet, "/x:worksheet/x:sheetFormatPr", "outlineLevelCol", "4");

    // First XML node, creates two columns (from min=1 to max=2)
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[1]", "hidden", "false");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[1]", "outlineLevel", "1");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[1]", "collapsed", "false");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[1]", "min", "1");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[1]", "max", "2");

    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[2]", "hidden", "true");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[2]", "outlineLevel", "2");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[2]", "collapsed", "false");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[2]", "min", "3");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[2]", "max", "3");

    // Column 4 has custom width and it is hidden. We need to make sure that it is created
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[3]", "hidden", "true");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[3]", "outlineLevel", "2");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[3]", "collapsed", "false");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[3]", "min", "4");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[3]", "max", "4");

    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[4]", "hidden", "true");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[4]", "outlineLevel", "3");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[4]", "collapsed", "false");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[4]", "min", "5");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[4]", "max", "6");

    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[5]", "hidden", "true");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[5]", "outlineLevel", "4");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[5]", "collapsed", "false");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[5]", "min", "7");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[5]", "max", "7");

    // Column 8 has custom width and it is hidden. We need to make sure that it is created
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[6]", "hidden", "true");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[6]", "outlineLevel", "4");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[6]", "collapsed", "false");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[6]", "min", "8");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[6]", "max", "8");

    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[7]", "hidden", "true");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[7]", "outlineLevel", "4");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[7]", "collapsed", "false");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[7]", "min", "9");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[7]", "max", "19");

    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[8]", "hidden", "true");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[8]", "outlineLevel", "3");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[8]", "collapsed", "true");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[8]", "min", "20");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[8]", "max", "20");

    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[9]", "hidden", "true");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[9]", "outlineLevel", "3");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[9]", "collapsed", "false");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[9]", "min", "21");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[9]", "max", "21");

    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[10]", "hidden", "true");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[10]", "outlineLevel", "2");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[10]", "collapsed", "false");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[10]", "min", "22");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[10]", "max", "23");

    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[11]", "hidden", "false");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[11]", "outlineLevel", "1");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[11]", "collapsed", "true");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[11]", "min", "24");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[11]", "max", "24");

    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[12]", "hidden", "false");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[12]", "outlineLevel", "1");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[12]", "collapsed", "false");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[12]", "min", "25");
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col[12]", "max", "26");

    // We expected that exactly 12 unique Nodes will be produced
    assertXPath(pSheet, "/x:worksheet/x:cols/x:col", 12);

    // First row is empty and default so it is not written into XML file
    // so we need to save 29 rows, as it provides information about outLineLevel
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[1]", "r", "2");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[1]", "hidden", "false");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[1]", "outlineLevel", "1");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[1]", "collapsed", "false");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[2]", "r", "3");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[2]", "hidden", "false");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[2]", "outlineLevel", "2");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[2]", "collapsed", "false");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[3]", "r", "4");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[3]", "hidden", "false");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[3]", "outlineLevel", "2");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[3]", "collapsed", "false");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[4]", "r", "5");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[4]", "hidden", "false");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[4]", "outlineLevel", "3");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[4]", "collapsed", "false");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[5]", "r", "6");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[5]", "hidden", "false");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[5]", "outlineLevel", "3");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[5]", "collapsed", "false");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[6]", "r", "7");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[6]", "hidden", "true");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[6]", "outlineLevel", "4");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[6]", "collapsed", "false");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[7]", "r", "8");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[7]", "hidden", "true");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[7]", "outlineLevel", "4");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[7]", "collapsed", "false");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[8]", "r", "9");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[8]", "hidden", "true");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[8]", "outlineLevel", "4");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[8]", "collapsed", "false");
    // Next rows are the same as the previous one but it needs to be preserved,
    // as they contain information about outlineLevel
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[20]", "r", "21");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[20]", "hidden", "true");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[20]", "outlineLevel", "4");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[20]", "collapsed", "false");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[21]", "r", "22");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[21]", "hidden", "false");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[21]", "outlineLevel", "3");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[21]", "collapsed", "true");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[22]", "r", "23");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[22]", "hidden", "false");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[22]", "outlineLevel", "3");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[22]", "collapsed", "false");

    // We expected that exactly 29 Row Nodes will be produced
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row", 29);
}

void ScExportTest::testAllRowsHiddenXLSX()
{
    createScDoc("xlsx/tdf105840_allRowsHidden.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pSheet = parseExport("xl/worksheets/sheet1.xml");
    CPPUNIT_ASSERT(pSheet);
    assertXPath(pSheet, "/x:worksheet/x:sheetFormatPr", "zeroHeight", "true");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row", 0);
}

void ScExportTest::testHiddenEmptyRowsXLSX()
{
    //tdf#98106 FILESAVE: Hidden and empty rows became visible when export to .XLSX
    createScDoc("ods/hidden-empty-rows.ods");

    save("Calc Office Open XML");
    xmlDocUniquePtr pSheet = parseExport("xl/worksheets/sheet1.xml");
    CPPUNIT_ASSERT(pSheet);

    assertXPath(pSheet, "/x:worksheet/x:sheetFormatPr", "zeroHeight", "false");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[1]", "hidden", "true");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[2]", "hidden", "true");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[3]", "hidden", "true");
    assertXPath(pSheet, "/x:worksheet/x:sheetData/x:row[4]", "hidden", "false");
}

void ScExportTest::testHiddenEmptyColsODS()
{
    //tdf#98106 FILESAVE: Hidden and empty rows became visible when export to .XLSX
    createScDoc("ods/tdf128895_emptyHiddenCols.ods");

    save("calc8");
    xmlDocUniquePtr pSheet = parseExport("content.xml");
    CPPUNIT_ASSERT(pSheet);
    assertXPath(pSheet, "//table:table/table:table-column[2]");
    assertXPath(pSheet, "//table:table/table:table-column[2]", "number-columns-repeated", "1017");
}

void ScExportTest::testLandscapeOrientationXLSX()
{
    //tdf#48767 - Landscape page orientation is not loaded from .xlsx format with MS Excel, after export with Libre Office
    createScDoc("ods/hidden-empty-rows.ods");

    save("Calc Office Open XML");
    xmlDocUniquePtr pSheet = parseExport("xl/worksheets/sheet1.xml");
    CPPUNIT_ASSERT(pSheet);

    // the usePrinterDefaults cannot be saved to allow opening sheets in Landscape mode via MS Excel
    assertXPathNoAttribute(pSheet, "/x:worksheet/x:pageSetup", "usePrinterDefaults");
    assertXPath(pSheet, "/x:worksheet/x:pageSetup", "orientation", "landscape");
}

void ScExportTest::testDataBarExportXLSX()
{
    createScDoc("xlsx/databar.xlsx");

    saveAndReload("Calc Office Open XML");

    ScDocument* pDoc = getScDoc();

    testDataBar_Impl(*pDoc);
}

void ScExportTest::testMiscRowHeightExport()
{
    static const TestParam::RowData DfltRowData[] = {
        { 0, 4, 0, 529, 0, false },
        { 5, 10, 0, 1058, 0, false },
        { 17, 20, 0, 1746, 0, false },
        // check last couple of row in document to ensure
        // they are 5.29mm ( effective default row xlsx height )
        { 1048573, 1048575, 0, 529, 0, false },
    };

    static const TestParam::RowData EmptyRepeatRowData[] = {
        // rows 0-4, 5-10, 17-20 are all set at various
        // heights, there is no content in the rows, there
        // was a bug where only the first row ( of repeated rows )
        // was set after export
        { 0, 4, 0, 529, 0, false },
        { 5, 10, 0, 1058, 0, false },
        { 17, 20, 0, 1767, 0, false },
    };

    TestParam aTestValues[] = {
        // Checks that some distributed ( non-empty ) heights remain set after export (roundtrip)
        // additionally there is effectively a default row height ( 5.29 mm ). So we test the
        // unset rows at the end of the document to ensure the effective xlsx default height
        // is set there too.
        { u"xlsx/miscrowheights.xlsx", "Calc Office Open XML", SAL_N_ELEMENTS(DfltRowData),
          DfltRowData },
        // Checks that some distributed ( non-empty ) heights remain set after export (to xls)
        { u"xlsx/miscrowheights.xlsx", "MS Excel 97", SAL_N_ELEMENTS(DfltRowData), DfltRowData },
        // Checks that repreated rows ( of various heights ) remain set after export ( to xlsx )
        { u"ods/miscemptyrepeatedrowheights.ods", "Calc Office Open XML",
          SAL_N_ELEMENTS(EmptyRepeatRowData), EmptyRepeatRowData },
        // Checks that repreated rows ( of various heights ) remain set after export ( to xls )
        { u"ods/miscemptyrepeatedrowheights.ods", "MS Excel 97", SAL_N_ELEMENTS(EmptyRepeatRowData),
          EmptyRepeatRowData },
    };
    miscRowHeightsTest(aTestValues, SAL_N_ELEMENTS(aTestValues));
}

namespace
{
void setAttribute(ScFieldEditEngine& rEE, sal_Int32 nPara, sal_Int32 nStart, sal_Int32 nEnd,
                  sal_uInt16 nType, Color nColor = COL_BLACK)
{
    ESelection aSel;
    aSel.nStartPara = aSel.nEndPara = nPara;
    aSel.nStartPos = nStart;
    aSel.nEndPos = nEnd;

    SfxItemSet aItemSet = rEE.GetEmptyItemSet();
    switch (nType)
    {
        case EE_CHAR_WEIGHT:
        {
            SvxWeightItem aWeight(WEIGHT_BOLD, nType);
            aItemSet.Put(aWeight);
            rEE.QuickSetAttribs(aItemSet, aSel);
        }
        break;
        case EE_CHAR_ITALIC:
        {
            SvxPostureItem aItalic(ITALIC_NORMAL, nType);
            aItemSet.Put(aItalic);
            rEE.QuickSetAttribs(aItemSet, aSel);
        }
        break;
        case EE_CHAR_STRIKEOUT:
        {
            SvxCrossedOutItem aCrossOut(STRIKEOUT_SINGLE, nType);
            aItemSet.Put(aCrossOut);
            rEE.QuickSetAttribs(aItemSet, aSel);
        }
        break;
        case EE_CHAR_OVERLINE:
        {
            SvxOverlineItem aItem(LINESTYLE_DOUBLE, nType);
            aItemSet.Put(aItem);
            rEE.QuickSetAttribs(aItemSet, aSel);
        }
        break;
        case EE_CHAR_UNDERLINE:
        {
            SvxUnderlineItem aItem(LINESTYLE_DOUBLE, nType);
            aItemSet.Put(aItem);
            rEE.QuickSetAttribs(aItemSet, aSel);
        }
        break;
        case EE_CHAR_COLOR:
        {
            SvxColorItem aItem(nColor, nType);
            aItemSet.Put(aItem);
            rEE.QuickSetAttribs(aItemSet, aSel);
        }
        break;
        default:;
    }
}

void setFont(ScFieldEditEngine& rEE, sal_Int32 nPara, sal_Int32 nStart, sal_Int32 nEnd,
             const OUString& rFontName)
{
    ESelection aSel;
    aSel.nStartPara = aSel.nEndPara = nPara;
    aSel.nStartPos = nStart;
    aSel.nEndPos = nEnd;

    SfxItemSet aItemSet = rEE.GetEmptyItemSet();
    SvxFontItem aItem(FAMILY_MODERN, rFontName, "", PITCH_VARIABLE, RTL_TEXTENCODING_UTF8,
                      EE_CHAR_FONTINFO);
    aItemSet.Put(aItem);
    rEE.QuickSetAttribs(aItemSet, aSel);
}

void setEscapement(ScFieldEditEngine& rEE, sal_Int32 nPara, sal_Int32 nStart, sal_Int32 nEnd,
                   short nEsc, sal_uInt8 nRelSize)
{
    ESelection aSel;
    aSel.nStartPara = aSel.nEndPara = nPara;
    aSel.nStartPos = nStart;
    aSel.nEndPos = nEnd;

    SfxItemSet aItemSet = rEE.GetEmptyItemSet();
    SvxEscapementItem aItem(nEsc, nRelSize, EE_CHAR_ESCAPEMENT);
    aItemSet.Put(aItem);
    rEE.QuickSetAttribs(aItemSet, aSel);
}
}

void ScExportTest::testNamedRangeBugfdo62729()
{
#if !defined(MACOSX) // FIXME: infinite loop on jenkins' mac
    createScDoc("ods/fdo62729.ods");
    ScDocument* pDoc = getScDoc();

    ScRangeName* pNames = pDoc->GetRangeName();
    //should be just a single named range
    CPPUNIT_ASSERT_EQUAL(size_t(1), pNames->size());
    pDoc->DeleteTab(0);
    //should be still a single named range
    CPPUNIT_ASSERT_EQUAL(size_t(1), pNames->size());
    saveAndReload("calc8");

    pDoc = getScDoc();

    pNames = pDoc->GetRangeName();
    //after reload should still have a named range
    CPPUNIT_ASSERT_EQUAL(size_t(1), pNames->size());
#endif
}

void ScExportTest::testBuiltinRangesXLSX()
{
    createScDoc("xlsx/built-in_ranges.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/workbook.xml");
    CPPUNIT_ASSERT(pDoc);

    //assert the existing OOXML built-in names are still there
    assertXPathContent(pDoc,
                       "/x:workbook/x:definedNames/"
                       "x:definedName[@name='_xlnm._FilterDatabase'][@localSheetId='0']",
                       "'Sheet1 Test'!$A$1:$A$5");
    assertXPathContent(pDoc,
                       "/x:workbook/x:definedNames/"
                       "x:definedName[@name='_xlnm._FilterDatabase'][@localSheetId='1']",
                       "'Sheet2 Test'!$K$10:$K$14");
    assertXPathContent(
        pDoc,
        "/x:workbook/x:definedNames/x:definedName[@name='_xlnm.Print_Area'][@localSheetId='0']",
        "'Sheet1 Test'!$A$1:$A$5");
    assertXPathContent(
        pDoc,
        "/x:workbook/x:definedNames/x:definedName[@name='_xlnm.Print_Area'][@localSheetId='1']",
        "'Sheet2 Test'!$K$10:$M$18");

    //...and that no extra ones are added (see tdf#112571)
    assertXPath(pDoc,
                "/x:workbook/x:definedNames/"
                "x:definedName[@name='_xlnm._FilterDatabase_0'][@localSheetId='0']",
                0);
    assertXPath(pDoc,
                "/x:workbook/x:definedNames/"
                "x:definedName[@name='_xlnm._FilterDatabase_0'][@localSheetId='1']",
                0);
    assertXPath(
        pDoc,
        "/x:workbook/x:definedNames/x:definedName[@name='_xlnm.Print_Area_0'][@localSheetId='0']",
        0);
    assertXPath(
        pDoc,
        "/x:workbook/x:definedNames/x:definedName[@name='_xlnm.Print_Area_0'][@localSheetId='1']",
        0);
}

void ScExportTest::testRichTextExportODS()
{
    struct
    {
        static bool isBold(const editeng::Section& rAttr)
        {
            return std::any_of(
                rAttr.maAttributes.begin(), rAttr.maAttributes.end(), [](const SfxPoolItem* p) {
                    return p->Which() == EE_CHAR_WEIGHT
                           && static_cast<const SvxWeightItem*>(p)->GetWeight() == WEIGHT_BOLD;
                });
        }

        static bool isItalic(const editeng::Section& rAttr)
        {
            return std::any_of(
                rAttr.maAttributes.begin(), rAttr.maAttributes.end(), [](const SfxPoolItem* p) {
                    return p->Which() == EE_CHAR_ITALIC
                           && static_cast<const SvxPostureItem*>(p)->GetPosture() == ITALIC_NORMAL;
                });
        }

        static bool isStrikeOut(const editeng::Section& rAttr)
        {
            return std::any_of(
                rAttr.maAttributes.begin(), rAttr.maAttributes.end(), [](const SfxPoolItem* p) {
                    return p->Which() == EE_CHAR_STRIKEOUT
                           && static_cast<const SvxCrossedOutItem*>(p)->GetStrikeout()
                                  == STRIKEOUT_SINGLE;
                });
        }

        static bool isOverline(const editeng::Section& rAttr, FontLineStyle eStyle)
        {
            return std::any_of(rAttr.maAttributes.begin(), rAttr.maAttributes.end(),
                               [&eStyle](const SfxPoolItem* p) {
                                   return p->Which() == EE_CHAR_OVERLINE
                                          && static_cast<const SvxOverlineItem*>(p)->GetLineStyle()
                                                 == eStyle;
                               });
        }

        static bool isUnderline(const editeng::Section& rAttr, FontLineStyle eStyle)
        {
            return std::any_of(rAttr.maAttributes.begin(), rAttr.maAttributes.end(),
                               [&eStyle](const SfxPoolItem* p) {
                                   return p->Which() == EE_CHAR_UNDERLINE
                                          && static_cast<const SvxUnderlineItem*>(p)->GetLineStyle()
                                                 == eStyle;
                               });
        }

        static bool isFont(const editeng::Section& rAttr, const OUString& rFontName)
        {
            return std::any_of(rAttr.maAttributes.begin(), rAttr.maAttributes.end(),
                               [&rFontName](const SfxPoolItem* p) {
                                   return p->Which() == EE_CHAR_FONTINFO
                                          && static_cast<const SvxFontItem*>(p)->GetFamilyName()
                                                 == rFontName;
                               });
        }

        static bool isEscapement(const editeng::Section& rAttr, short nEsc, sal_uInt8 nRelSize)
        {
            return std::any_of(rAttr.maAttributes.begin(), rAttr.maAttributes.end(),
                               [&nEsc, &nRelSize](const SfxPoolItem* p) {
                                   if (p->Which() != EE_CHAR_ESCAPEMENT)
                                       return false;
                                   const SvxEscapementItem* pItem
                                       = static_cast<const SvxEscapementItem*>(p);
                                   return ((pItem->GetEsc() == nEsc)
                                           && (pItem->GetProportionalHeight() == nRelSize));
                               });
        }

        static bool isColor(const editeng::Section& rAttr, Color nColor)
        {
            return std::any_of(rAttr.maAttributes.begin(), rAttr.maAttributes.end(),
                               [&nColor](const SfxPoolItem* p) {
                                   return p->Which() == EE_CHAR_COLOR
                                          && static_cast<const SvxColorItem*>(p)->GetValue()
                                                 == nColor;
                               });
        }

        bool checkB2(const EditTextObject* pText) const
        {
            if (!pText)
                return false;

            if (pText->GetParagraphCount() != 1)
                return false;

            if (pText->GetText(0) != "Bold and Italic")
                return false;

            std::vector<editeng::Section> aSecAttrs;
            pText->GetAllSections(aSecAttrs);
            if (aSecAttrs.size() != 3)
                return false;

            // Check the first bold section.
            const editeng::Section* pAttr = aSecAttrs.data();
            if (pAttr->mnParagraph != 0 || pAttr->mnStart != 0 || pAttr->mnEnd != 4)
                return false;

            if (pAttr->maAttributes.size() != 1 || !isBold(*pAttr))
                return false;

            // The middle section should be unformatted.
            pAttr = &aSecAttrs[1];
            if (pAttr->mnParagraph != 0 || pAttr->mnStart != 4 || pAttr->mnEnd != 9)
                return false;

            if (!pAttr->maAttributes.empty())
                return false;

            // The last section should be italic.
            pAttr = &aSecAttrs[2];
            if (pAttr->mnParagraph != 0 || pAttr->mnStart != 9 || pAttr->mnEnd != 15)
                return false;

            if (pAttr->maAttributes.size() != 1 || !isItalic(*pAttr))
                return false;

            return true;
        }

        bool checkB4(const EditTextObject* pText) const
        {
            if (!pText)
                return false;

            if (pText->GetParagraphCount() != 3)
                return false;

            if (pText->GetText(0) != "One")
                return false;

            if (pText->GetText(1) != "Two")
                return false;

            if (pText->GetText(2) != "Three")
                return false;

            return true;
        }

        bool checkB5(const EditTextObject* pText) const
        {
            if (!pText)
                return false;

            if (pText->GetParagraphCount() != 6)
                return false;

            if (!pText->GetText(0).isEmpty())
                return false;

            if (pText->GetText(1) != "Two")
                return false;

            if (pText->GetText(2) != "Three")
                return false;

            if (!pText->GetText(3).isEmpty())
                return false;

            if (pText->GetText(4) != "Five")
                return false;

            if (!pText->GetText(5).isEmpty())
                return false;

            return true;
        }

        bool checkB6(const EditTextObject* pText) const
        {
            if (!pText)
                return false;

            if (pText->GetParagraphCount() != 1)
                return false;

            if (pText->GetText(0) != "Strike Me")
                return false;

            std::vector<editeng::Section> aSecAttrs;
            pText->GetAllSections(aSecAttrs);
            if (aSecAttrs.size() != 2)
                return false;

            // Check the first strike-out section.
            const editeng::Section* pAttr = aSecAttrs.data();
            if (pAttr->mnParagraph != 0 || pAttr->mnStart != 0 || pAttr->mnEnd != 6)
                return false;

            if (pAttr->maAttributes.size() != 1 || !isStrikeOut(*pAttr))
                return false;

            // The last section should be unformatted.
            pAttr = &aSecAttrs[1];
            return pAttr->mnParagraph == 0 && pAttr->mnStart == 6 && pAttr->mnEnd == 9;
        }

        bool checkB7(const EditTextObject* pText) const
        {
            if (!pText)
                return false;

            if (pText->GetParagraphCount() != 1)
                return false;

            if (pText->GetText(0) != "Font1 and Font2")
                return false;

            std::vector<editeng::Section> aSecAttrs;
            pText->GetAllSections(aSecAttrs);
            if (aSecAttrs.size() != 3)
                return false;

            // First section should have "Courier" font applied.
            const editeng::Section* pAttr = aSecAttrs.data();
            if (pAttr->mnParagraph != 0 || pAttr->mnStart != 0 || pAttr->mnEnd != 5)
                return false;

            if (pAttr->maAttributes.size() != 1 || !isFont(*pAttr, "Courier"))
                return false;

            // Last section should have "Luxi Mono" applied.
            pAttr = &aSecAttrs[2];
            if (pAttr->mnParagraph != 0 || pAttr->mnStart != 10 || pAttr->mnEnd != 15)
                return false;

            if (pAttr->maAttributes.size() != 1 || !isFont(*pAttr, "Luxi Mono"))
                return false;

            return true;
        }

        bool checkB8(const EditTextObject* pText) const
        {
            if (!pText)
                return false;

            if (pText->GetParagraphCount() != 1)
                return false;

            if (pText->GetText(0) != "Over and Under")
                return false;

            std::vector<editeng::Section> aSecAttrs;
            pText->GetAllSections(aSecAttrs);
            if (aSecAttrs.size() != 3)
                return false;

            // First section should have overline applied.
            const editeng::Section* pAttr = aSecAttrs.data();
            if (pAttr->mnParagraph != 0 || pAttr->mnStart != 0 || pAttr->mnEnd != 4)
                return false;

            if (pAttr->maAttributes.size() != 1 || !isOverline(*pAttr, LINESTYLE_DOUBLE))
                return false;

            // Last section should have underline applied.
            pAttr = &aSecAttrs[2];
            if (pAttr->mnParagraph != 0 || pAttr->mnStart != 9 || pAttr->mnEnd != 14)
                return false;

            if (pAttr->maAttributes.size() != 1 || !isUnderline(*pAttr, LINESTYLE_DOUBLE))
                return false;

            return true;
        }

        bool checkB9(const EditTextObject* pText) const
        {
            if (!pText)
                return false;

            if (pText->GetParagraphCount() != 1)
                return false;

            if (pText->GetText(0) != "Sub and Super")
                return false;

            std::vector<editeng::Section> aSecAttrs;
            pText->GetAllSections(aSecAttrs);
            if (aSecAttrs.size() != 3)
                return false;

            // superscript
            const editeng::Section* pAttr = aSecAttrs.data();
            if (pAttr->mnParagraph != 0 || pAttr->mnStart != 0 || pAttr->mnEnd != 3)
                return false;

            if (pAttr->maAttributes.size() != 1 || !isEscapement(*pAttr, 32, 64))
                return false;

            // subscript
            pAttr = &aSecAttrs[2];
            if (pAttr->mnParagraph != 0 || pAttr->mnStart != 8 || pAttr->mnEnd != 13)
                return false;

            if (pAttr->maAttributes.size() != 1 || !isEscapement(*pAttr, -32, 66))
                return false;

            return true;
        }

        bool checkB10(const EditTextObject* pText) const
        {
            if (!pText)
                return false;

            if (pText->GetParagraphCount() != 1)
                return false;

            if (pText->GetText(0) != "BLUE AUTO")
                return false;

            std::vector<editeng::Section> aSecAttrs;
            pText->GetAllSections(aSecAttrs);
            if (aSecAttrs.size() != 2)
                return false;

            // auto color
            const editeng::Section* pAttr = &aSecAttrs[1];
            if (pAttr->mnParagraph != 0 || pAttr->mnStart != 5 || pAttr->mnEnd != 9)
                return false;

            if (pAttr->maAttributes.size() != 1 || !isColor(*pAttr, COL_AUTO))
                return false;

            return true;
        }

    } aCheckFunc;

    // Start with an empty document, put one edit text cell, and make sure it
    // survives the save and reload.
    createScDoc();
    const EditTextObject* pEditText;
    {
        ScDocument* pDoc = getScDoc();
        CPPUNIT_ASSERT_MESSAGE("This document should at least have one sheet.",
                               pDoc->GetTableCount() > 0);

        // Insert an edit text cell.
        ScFieldEditEngine* pEE = &pDoc->GetEditEngine();
        pEE->SetTextCurrentDefaults("Bold and Italic");
        // Set the 'Bold' part bold.
        setAttribute(*pEE, 0, 0, 4, EE_CHAR_WEIGHT);
        // Set the 'Italic' part italic.
        setAttribute(*pEE, 0, 9, 15, EE_CHAR_ITALIC);
        ESelection aSel;
        aSel.nStartPara = aSel.nEndPara = 0;

        // Set this edit text to cell B2.
        pDoc->SetEditText(ScAddress(1, 1, 0), pEE->CreateTextObject());
        pEditText = pDoc->GetEditText(ScAddress(1, 1, 0));
        CPPUNIT_ASSERT_MESSAGE("Incorrect B2 value.", aCheckFunc.checkB2(pEditText));
    }

    // Now, save and reload this document.
    saveAndReload("calc8");
    {
        ScDocument* pDoc = getScDoc();
        CPPUNIT_ASSERT_MESSAGE("Reloaded document should at least have one sheet.",
                               pDoc->GetTableCount() > 0);
        ScFieldEditEngine* pEE = &pDoc->GetEditEngine();
        pEditText = pDoc->GetEditText(ScAddress(1, 1, 0));

        // Make sure the content of B2 is still intact.
        CPPUNIT_ASSERT_MESSAGE("Incorrect B2 value.", aCheckFunc.checkB2(pEditText));

        // Insert a multi-line content to B4.
        pEE->Clear();
        pEE->SetTextCurrentDefaults("One\nTwo\nThree");
        pDoc->SetEditText(ScAddress(1, 3, 0), pEE->CreateTextObject());
        pEditText = pDoc->GetEditText(ScAddress(1, 3, 0));
        CPPUNIT_ASSERT_MESSAGE("Incorrect B4 value.", aCheckFunc.checkB4(pEditText));
    }

    // Reload the doc again, and check the content of B2 and B4.
    saveAndReload("calc8");
    {
        ScDocument* pDoc = getScDoc();
        ScFieldEditEngine* pEE = &pDoc->GetEditEngine();

        pEditText = pDoc->GetEditText(ScAddress(1, 1, 0));
        CPPUNIT_ASSERT_MESSAGE("B2 should be an edit text.", pEditText);
        pEditText = pDoc->GetEditText(ScAddress(1, 3, 0));
        CPPUNIT_ASSERT_MESSAGE("Incorrect B4 value.", aCheckFunc.checkB4(pEditText));

        // Insert a multi-line content to B5, but this time, set some empty paragraphs.
        pEE->Clear();
        pEE->SetTextCurrentDefaults("\nTwo\nThree\n\nFive\n");
        pDoc->SetEditText(ScAddress(1, 4, 0), pEE->CreateTextObject());
        pEditText = pDoc->GetEditText(ScAddress(1, 4, 0));
        CPPUNIT_ASSERT_MESSAGE("Incorrect B5 value.", aCheckFunc.checkB5(pEditText));

        // Insert a text with strikethrough in B6.
        pEE->Clear();
        pEE->SetTextCurrentDefaults("Strike Me");
        // Set the 'Strike' part strikethrough.
        setAttribute(*pEE, 0, 0, 6, EE_CHAR_STRIKEOUT);
        pDoc->SetEditText(ScAddress(1, 5, 0), pEE->CreateTextObject());
        pEditText = pDoc->GetEditText(ScAddress(1, 5, 0));
        CPPUNIT_ASSERT_MESSAGE("Incorrect B6 value.", aCheckFunc.checkB6(pEditText));

        // Insert a text with different font segments in B7.
        pEE->Clear();
        pEE->SetTextCurrentDefaults("Font1 and Font2");
        setFont(*pEE, 0, 0, 5, "Courier");
        setFont(*pEE, 0, 10, 15, "Luxi Mono");
        pDoc->SetEditText(ScAddress(1, 6, 0), pEE->CreateTextObject());
        pEditText = pDoc->GetEditText(ScAddress(1, 6, 0));
        CPPUNIT_ASSERT_MESSAGE("Incorrect B7 value.", aCheckFunc.checkB7(pEditText));

        // Insert a text with overline and underline in B8.
        pEE->Clear();
        pEE->SetTextCurrentDefaults("Over and Under");
        setAttribute(*pEE, 0, 0, 4, EE_CHAR_OVERLINE);
        setAttribute(*pEE, 0, 9, 14, EE_CHAR_UNDERLINE);
        pDoc->SetEditText(ScAddress(1, 7, 0), pEE->CreateTextObject());
        pEditText = pDoc->GetEditText(ScAddress(1, 7, 0));
        CPPUNIT_ASSERT_MESSAGE("Incorrect B8 value.", aCheckFunc.checkB8(pEditText));

        pEE->Clear();
        pEE->SetTextCurrentDefaults("Sub and Super");
        setEscapement(*pEE, 0, 0, 3, 32, 64);
        setEscapement(*pEE, 0, 8, 13, -32, 66);
        pDoc->SetEditText(ScAddress(1, 8, 0), pEE->CreateTextObject());
        pEditText = pDoc->GetEditText(ScAddress(1, 8, 0));
        CPPUNIT_ASSERT_MESSAGE("Incorrect B9 value.", aCheckFunc.checkB9(pEditText));

        ScPatternAttr aCellFontColor(pDoc->GetPool());
        aCellFontColor.GetItemSet().Put(SvxColorItem(COL_BLUE, ATTR_FONT_COLOR));
        // Set font color of B10 to blue.
        pDoc->ApplyPattern(1, 9, 0, aCellFontColor);
        pEE->Clear();
        pEE->SetTextCurrentDefaults("BLUE AUTO");
        // Set the color of the string "AUTO" to automatic color.
        setAttribute(*pEE, 0, 5, 9, EE_CHAR_COLOR, COL_AUTO);
        pDoc->SetEditText(ScAddress(1, 9, 0), pEE->CreateTextObject());
        pEditText = pDoc->GetEditText(ScAddress(1, 9, 0));
        CPPUNIT_ASSERT_MESSAGE("Incorrect B10 value.", aCheckFunc.checkB10(pEditText));
    }

    // Reload the doc again, and check the content of B2, B4, B6 and B7.
    saveAndReload("calc8");
    ScDocument* pDoc = getScDoc();

    pEditText = pDoc->GetEditText(ScAddress(1, 1, 0));
    CPPUNIT_ASSERT_MESSAGE("Incorrect B2 value after save and reload.",
                           aCheckFunc.checkB2(pEditText));
    pEditText = pDoc->GetEditText(ScAddress(1, 3, 0));
    CPPUNIT_ASSERT_MESSAGE("Incorrect B4 value after save and reload.",
                           aCheckFunc.checkB4(pEditText));
    pEditText = pDoc->GetEditText(ScAddress(1, 4, 0));
    CPPUNIT_ASSERT_MESSAGE("Incorrect B5 value after save and reload.",
                           aCheckFunc.checkB5(pEditText));
    pEditText = pDoc->GetEditText(ScAddress(1, 5, 0));
    CPPUNIT_ASSERT_MESSAGE("Incorrect B6 value after save and reload.",
                           aCheckFunc.checkB6(pEditText));
    pEditText = pDoc->GetEditText(ScAddress(1, 6, 0));
    CPPUNIT_ASSERT_MESSAGE("Incorrect B7 value after save and reload.",
                           aCheckFunc.checkB7(pEditText));
    pEditText = pDoc->GetEditText(ScAddress(1, 7, 0));
    CPPUNIT_ASSERT_MESSAGE("Incorrect B8 value after save and reload.",
                           aCheckFunc.checkB8(pEditText));
    pEditText = pDoc->GetEditText(ScAddress(1, 9, 0));
    CPPUNIT_ASSERT_MESSAGE("Incorrect B10 value after save and reload.",
                           aCheckFunc.checkB10(pEditText));
}

void ScExportTest::testRichTextCellFormatXLSX()
{
    createScDoc("xls/cellformat.xls");

    save("Calc Office Open XML");
    xmlDocUniquePtr pSheet = parseExport("xl/worksheets/sheet1.xml");
    CPPUNIT_ASSERT(pSheet);

    // make sure the only cell in this doc is assigned some formatting record
    OUString aCellFormat = getXPath(pSheet, "/x:worksheet/x:sheetData/x:row/x:c", "s");
    CPPUNIT_ASSERT_MESSAGE("Cell format is missing", !aCellFormat.isEmpty());

    xmlDocUniquePtr pStyles = parseExport("xl/styles.xml");
    CPPUNIT_ASSERT(pStyles);

    OString nFormatIdx = OString::number(aCellFormat.toInt32() + 1);
    const OString aXPath1("/x:styleSheet/x:cellXfs/x:xf[" + nFormatIdx + "]/x:alignment");
    // formatting record is set to wrap text
    assertXPath(pStyles, aXPath1, "wrapText", "true");

    // see what font it references
    const OString aXPath2("/x:styleSheet/x:cellXfs/x:xf[" + nFormatIdx + "]");
    OUString aFontId = getXPath(pStyles, aXPath2, "fontId");
    OString nFontIdx = OString::number(aFontId.toInt32() + 1);

    // that font should be bold
    const OString aXPath3("/x:styleSheet/x:fonts/x:font[" + nFontIdx + "]/x:b");
    assertXPath(pStyles, aXPath3, "val", "true");
}

void ScExportTest::testFormulaRefSheetNameODS()
{
    createScDoc("ods/formula-quote-in-sheet-name.ods");
    {
        ScDocument* pDoc = getScDoc();

        sc::AutoCalcSwitch aACSwitch(*pDoc, true); // turn on auto calc.
        pDoc->SetString(ScAddress(1, 1, 0), "='90''s Data'.B2");
        CPPUNIT_ASSERT_EQUAL(1.1, pDoc->GetValue(ScAddress(1, 1, 0)));
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong formula", OUString("='90''s Data'.B2"),
                                     pDoc->GetFormula(1, 1, 0));
    }
    // Now, save and reload this document.
    saveAndReload("calc8");

    ScDocument* pDoc = getScDoc();
    pDoc->CalcAll();
    CPPUNIT_ASSERT_EQUAL(1.1, pDoc->GetValue(ScAddress(1, 1, 0)));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong formula", OUString("='90''s Data'.B2"),
                                 pDoc->GetFormula(1, 1, 0));
}

void ScExportTest::testCellValuesExportODS()
{
    // Start with an empty document
    createScDoc();
    {
        ScDocument* pDoc = getScDoc();
        CPPUNIT_ASSERT_MESSAGE("This document should at least have one sheet.",
                               pDoc->GetTableCount() > 0);

        // set a value double
        pDoc->SetValue(ScAddress(0, 0, 0), 2.0); // A1

        // set a formula
        pDoc->SetValue(ScAddress(2, 0, 0), 3.0); // C1
        pDoc->SetValue(ScAddress(3, 0, 0), 3); // D1
        pDoc->SetString(ScAddress(4, 0, 0), "=10*C1/4"); // E1
        pDoc->SetValue(ScAddress(5, 0, 0), 3.0); // F1
        pDoc->SetString(ScAddress(7, 0, 0), "=SUM(C1:F1)"); //H1

        // set a string
        pDoc->SetString(ScAddress(0, 2, 0), "a simple line"); //A3

        // set a digit string
        pDoc->SetString(ScAddress(0, 4, 0), "'12"); //A5
        // set a contiguous value
        pDoc->SetValue(ScAddress(0, 5, 0), 12.0); //A6
        // set a contiguous string
        pDoc->SetString(ScAddress(0, 6, 0), "a string"); //A7
        // set a contiguous formula
        pDoc->SetString(ScAddress(0, 7, 0), "=$A$6"); //A8
    }
    // save and reload
    saveAndReload("calc8");
    ScDocument* pDoc = getScDoc();
    CPPUNIT_ASSERT_MESSAGE("Reloaded document should at least have one sheet.",
                           pDoc->GetTableCount() > 0);

    // check value
    CPPUNIT_ASSERT_EQUAL(2.0, pDoc->GetValue(0, 0, 0));
    CPPUNIT_ASSERT_EQUAL(3.0, pDoc->GetValue(2, 0, 0));
    CPPUNIT_ASSERT_EQUAL(3.0, pDoc->GetValue(3, 0, 0));
    CPPUNIT_ASSERT_EQUAL(7.5, pDoc->GetValue(4, 0, 0));
    CPPUNIT_ASSERT_EQUAL(3.0, pDoc->GetValue(5, 0, 0));

    // check formula
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong formula =10*C1/4", OUString("=10*C1/4"),
                                 pDoc->GetFormula(4, 0, 0));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong formula =SUM(C1:F1)", OUString("=SUM(C1:F1)"),
                                 pDoc->GetFormula(7, 0, 0));
    CPPUNIT_ASSERT_EQUAL(16.5, pDoc->GetValue(7, 0, 0));

    // check string
    ScRefCellValue aCell;
    aCell.assign(*pDoc, ScAddress(0, 2, 0));
    CPPUNIT_ASSERT_EQUAL(CELLTYPE_STRING, aCell.getType());

    // check for an empty cell
    aCell.assign(*pDoc, ScAddress(0, 3, 0));
    CPPUNIT_ASSERT_EQUAL(CELLTYPE_NONE, aCell.getType());

    // check a digit string
    aCell.assign(*pDoc, ScAddress(0, 4, 0));
    CPPUNIT_ASSERT_EQUAL(CELLTYPE_STRING, aCell.getType());

    //check contiguous values
    CPPUNIT_ASSERT_EQUAL(12.0, pDoc->GetValue(0, 5, 0));
    CPPUNIT_ASSERT_EQUAL(OUString("a string"), pDoc->GetString(0, 6, 0));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong formula =$A$6", OUString("=$A$6"),
                                 pDoc->GetFormula(0, 7, 0));
    CPPUNIT_ASSERT_EQUAL(pDoc->GetValue(0, 5, 0), pDoc->GetValue(0, 7, 0));
}

void ScExportTest::testCellNoteExportODS()
{
    createScDoc("ods/single-note.ods");
    ScAddress aPos(0, 0, 0); // Start with A1.
    {
        ScDocument* pDoc = getScDoc();

        CPPUNIT_ASSERT_MESSAGE("There should be a note at A1.", pDoc->HasNote(aPos));

        aPos.IncRow(); // Move to A2.
        ScPostIt* pNote = pDoc->GetOrCreateNote(aPos);
        pNote->SetText(aPos, "Note One");
        pNote->SetAuthor("Author One");
        CPPUNIT_ASSERT_MESSAGE("There should be a note at A2.", pDoc->HasNote(aPos));
    }
    // save and reload
    saveAndReload("calc8");
    ScDocument* pDoc = getScDoc();

    aPos.SetRow(0); // Move back to A1.
    CPPUNIT_ASSERT_MESSAGE("There should be a note at A1.", pDoc->HasNote(aPos));
    aPos.IncRow(); // Move to A2.
    CPPUNIT_ASSERT_MESSAGE("There should be a note at A2.", pDoc->HasNote(aPos));
}

void ScExportTest::testCellNoteExportXLS()
{
    // Start with an empty document.s
    createScDoc("ods/notes-on-3-sheets.ods");
    {
        ScDocument* pDoc = getScDoc();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("This document should have 3 sheets.", SCTAB(3),
                                     pDoc->GetTableCount());

        // Check note's presence.
        CPPUNIT_ASSERT(pDoc->HasNote(ScAddress(0, 0, 0)));
        CPPUNIT_ASSERT(!pDoc->HasNote(ScAddress(0, 1, 0)));
        CPPUNIT_ASSERT(!pDoc->HasNote(ScAddress(0, 2, 0)));

        CPPUNIT_ASSERT(!pDoc->HasNote(ScAddress(0, 0, 1)));
        CPPUNIT_ASSERT(pDoc->HasNote(ScAddress(0, 1, 1)));
        CPPUNIT_ASSERT(!pDoc->HasNote(ScAddress(0, 2, 1)));

        CPPUNIT_ASSERT(!pDoc->HasNote(ScAddress(0, 0, 2)));
        CPPUNIT_ASSERT(!pDoc->HasNote(ScAddress(0, 1, 2)));
        CPPUNIT_ASSERT(pDoc->HasNote(ScAddress(0, 2, 2)));
    }
    // save and reload as XLS.
    saveAndReload("MS Excel 97");
    {
        ScDocument* pDoc = getScDoc();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("This document should have 3 sheets.", SCTAB(3),
                                     pDoc->GetTableCount());

        // Check note's presence again.
        CPPUNIT_ASSERT(pDoc->HasNote(ScAddress(0, 0, 0)));
        CPPUNIT_ASSERT(!pDoc->HasNote(ScAddress(0, 1, 0)));
        CPPUNIT_ASSERT(!pDoc->HasNote(ScAddress(0, 2, 0)));

        CPPUNIT_ASSERT(!pDoc->HasNote(ScAddress(0, 0, 1)));
        CPPUNIT_ASSERT(pDoc->HasNote(ScAddress(0, 1, 1)));
        CPPUNIT_ASSERT(!pDoc->HasNote(ScAddress(0, 2, 1)));

        CPPUNIT_ASSERT(!pDoc->HasNote(ScAddress(0, 0, 2)));
        CPPUNIT_ASSERT(!pDoc->HasNote(ScAddress(0, 1, 2)));
        CPPUNIT_ASSERT(pDoc->HasNote(ScAddress(0, 2, 2)));
    }
}

namespace
{
void checkMatrixRange(ScDocument& rDoc, const ScRange& rRange)
{
    ScRange aMatRange;
    ScAddress aMatOrigin;
    for (SCCOL nCol = rRange.aStart.Col(); nCol <= rRange.aEnd.Col(); ++nCol)
    {
        for (SCROW nRow = rRange.aStart.Row(); nRow <= rRange.aEnd.Row(); ++nRow)
        {
            ScAddress aPos(nCol, nRow, rRange.aStart.Tab());
            bool bIsMatrix = rDoc.GetMatrixFormulaRange(aPos, aMatRange);
            CPPUNIT_ASSERT_MESSAGE("Matrix expected, but not found.", bIsMatrix);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong matrix range.", rRange, aMatRange);
            const ScFormulaCell* pCell = rDoc.GetFormulaCell(aPos);
            CPPUNIT_ASSERT_MESSAGE("This must be a formula cell.", pCell);

            bIsMatrix = pCell->GetMatrixOrigin(rDoc, aMatOrigin);
            CPPUNIT_ASSERT_MESSAGE("Not a part of matrix formula.", bIsMatrix);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong matrix origin.", aMatRange.aStart, aMatOrigin);
        }
    }
}
}

void ScExportTest::testInlineArrayXLS()
{
    createScDoc("xls/inline-array.xls");

    saveAndReload("MS Excel 97");

    ScDocument* pDoc = getScDoc();

    // B2:C3 contains a matrix.
    checkMatrixRange(*pDoc, ScRange(1, 1, 0, 2, 2, 0));

    // B5:D6 contains a matrix.
    checkMatrixRange(*pDoc, ScRange(1, 4, 0, 3, 5, 0));

    // B8:C10 as well.
    checkMatrixRange(*pDoc, ScRange(1, 7, 0, 2, 9, 0));
}

void ScExportTest::testEmbeddedChartODS()
{
    createScDoc("xls/embedded-chart.xls");

    save("calc8");

    xmlDocUniquePtr pDoc = parseExport("content.xml");
    CPPUNIT_ASSERT(pDoc);
    assertXPath(pDoc,
                "/office:document-content/office:body/office:spreadsheet/table:table[2]/"
                "table:table-row[7]/table:table-cell[2]/draw:frame/draw:object",
                "notify-on-update-of-ranges",
                "Chart1.B3:Chart1.B5 Chart1.C2:Chart1.C2 Chart1.C3:Chart1.C5");
}

void ScExportTest::testEmbeddedChartXLS()
{
    createScDoc("xls/embedded-chart.xls");

    saveAndReload("MS Excel 97");

    ScDocument* pDoc = getScDoc();

    // Make sure the 2nd sheet is named 'Chart1'.
    OUString aName;
    pDoc->GetName(1, aName);
    CPPUNIT_ASSERT_EQUAL(OUString("Chart1"), aName);

    const SdrOle2Obj* pOleObj = getSingleChartObject(*pDoc, 1);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve a chart object from the 2nd sheet.", pOleObj);

    ScRangeList aRanges = getChartRanges(*pDoc, *pOleObj);
    CPPUNIT_ASSERT_MESSAGE("Label range (B3:B5) not found.",
                           aRanges.Contains(ScRange(1, 2, 1, 1, 4, 1)));
    CPPUNIT_ASSERT_MESSAGE("Data label (C2) not found.", aRanges.Contains(ScAddress(2, 1, 1)));
    CPPUNIT_ASSERT_MESSAGE("Data range (C3:C5) not found.",
                           aRanges.Contains(ScRange(2, 2, 1, 2, 4, 1)));
}

void ScExportTest::testCellAnchoredGroupXLS()
{
    createScDoc("xls/cell-anchored-group.xls");

    saveAndReload("calc8");

    // the document contains a group anchored on the first cell, make sure it's there in the right place
    ScDocument* pDoc = getScDoc();
    CPPUNIT_ASSERT_MESSAGE("There should be at least one sheet.", pDoc->GetTableCount() > 0);
    ScDrawLayer* pDrawLayer = pDoc->GetDrawLayer();
    SdrPage* pPage = pDrawLayer->GetPage(0);
    CPPUNIT_ASSERT_MESSAGE("draw page for sheet 1 should exist.", pPage);
    const size_t nCount = pPage->GetObjCount();
    CPPUNIT_ASSERT_EQUAL_MESSAGE("There should be 1 objects.", static_cast<size_t>(1), nCount);

    SdrObject* pObj = pPage->GetObj(0);
    CPPUNIT_ASSERT_MESSAGE("Failed to get drawing object.", pObj);
    ScDrawObjData* pData = ScDrawLayer::GetObjData(pObj);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve user data for this object.", pData);
    CPPUNIT_ASSERT_MESSAGE("Upper left of bounding rectangle should be nonnegative.",
                           pData->getShapeRect().Left() >= 0 || pData->getShapeRect().Top() >= 0);
}

void ScExportTest::testFormulaReferenceXLS()
{
    createScDoc("xls/formula-reference.xls");

    saveAndReload("MS Excel 97");

    ScDocument* pDoc = getScDoc();

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong formula in D2", OUString("=$A$2+$B$2+$C$2"),
                                 pDoc->GetFormula(3, 1, 0));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong formula in D3", OUString("=A3+B3+C3"),
                                 pDoc->GetFormula(3, 2, 0));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong formula in D6", OUString("=SUM($A$6:$C$6)"),
                                 pDoc->GetFormula(3, 5, 0));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong formula in D7", OUString("=SUM(A7:C7)"),
                                 pDoc->GetFormula(3, 6, 0));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong formula in D10", OUString("=$Two.$A$2+$Two.$B$2+$Two.$C$2"),
                                 pDoc->GetFormula(3, 9, 0));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong formula in D11", OUString("=$Two.A3+$Two.B3+$Two.C3"),
                                 pDoc->GetFormula(3, 10, 0));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong formula in D14", OUString("=MIN($Two.$A$2:$C$2)"),
                                 pDoc->GetFormula(3, 13, 0));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong formula in D15", OUString("=MAX($Two.A3:C3)"),
                                 pDoc->GetFormula(3, 14, 0));
}

void ScExportTest::testSheetProtectionXLSX()
{
    createScDoc("xlsx/ProtecteSheet1234Pass.xlsx");

    saveAndReload("Calc Office Open XML");

    ScDocument* pDoc = getScDoc();
    const ScTableProtection* pTabProtect = pDoc->GetTabProtection(0);
    CPPUNIT_ASSERT(pTabProtect);
    Sequence<sal_Int8> aHash = pTabProtect->getPasswordHash(PASSHASH_XL);
    // check has
    if (aHash.getLength() >= 2)
    {
        CPPUNIT_ASSERT_EQUAL(sal_uInt8(204), static_cast<sal_uInt8>(aHash[0]));
        CPPUNIT_ASSERT_EQUAL(sal_uInt8(61), static_cast<sal_uInt8>(aHash[1]));
    }
    // we could flesh out this check I guess
    CPPUNIT_ASSERT(!pTabProtect->isOptionEnabled(ScTableProtection::OBJECTS));
    CPPUNIT_ASSERT(!pTabProtect->isOptionEnabled(ScTableProtection::SCENARIOS));
}

void ScExportTest::testSheetProtectionXLSB()
{
    createScDoc("xlsb/tdf108017_calcProtection.xlsb");

    saveAndReload("Calc Office Open XML");

    ScDocument* pDoc = getScDoc();
    const ScTableProtection* pTabProtect = pDoc->GetTabProtection(0);
    CPPUNIT_ASSERT(pTabProtect);
    CPPUNIT_ASSERT(pTabProtect->isOptionEnabled(ScTableProtection::SELECT_UNLOCKED_CELLS));
    CPPUNIT_ASSERT(!pTabProtect->isOptionEnabled(ScTableProtection::SELECT_LOCKED_CELLS));
}

namespace
{
const char* toBorderName(SvxBorderLineStyle eStyle)
{
    switch (eStyle)
    {
        case SvxBorderLineStyle::SOLID:
            return "SOLID";
        case SvxBorderLineStyle::DOTTED:
            return "DOTTED";
        case SvxBorderLineStyle::DASHED:
            return "DASHED";
        case SvxBorderLineStyle::DASH_DOT:
            return "DASH_DOT";
        case SvxBorderLineStyle::DASH_DOT_DOT:
            return "DASH_DOT_DOT";
        case SvxBorderLineStyle::DOUBLE_THIN:
            return "DOUBLE_THIN";
        case SvxBorderLineStyle::FINE_DASHED:
            return "FINE_DASHED";
        default:;
    }

    return "";
}
}

void ScExportTest::testExcelCellBorders(const OUString& sFormatType)
{
    static const struct
    {
        SCROW mnRow;
        SvxBorderLineStyle mnStyle;
        tools::Long mnWidth;
    } aChecks[] = {
        { 1, SvxBorderLineStyle::SOLID, 1 }, // hair
        { 3, SvxBorderLineStyle::DOTTED, 15 }, // dotted
        { 5, SvxBorderLineStyle::DASH_DOT_DOT, 15 }, // dash dot dot
        { 7, SvxBorderLineStyle::DASH_DOT, 15 }, // dash dot
        { 9, SvxBorderLineStyle::FINE_DASHED, 15 }, // dashed
        { 11, SvxBorderLineStyle::SOLID, 15 }, // thin
        { 13, SvxBorderLineStyle::DASH_DOT_DOT, 35 }, // medium dash dot dot
        { 17, SvxBorderLineStyle::DASH_DOT, 35 }, // medium dash dot
        { 19, SvxBorderLineStyle::DASHED, 35 }, // medium dashed
        { 21, SvxBorderLineStyle::SOLID, 35 }, // medium
        { 23, SvxBorderLineStyle::SOLID, 50 }, // thick
        { 25, SvxBorderLineStyle::DOUBLE_THIN, -1 }, // double (don't check width)
    };

    {
        ScDocument* pDoc = getScDoc();

        for (size_t i = 0; i < SAL_N_ELEMENTS(aChecks); ++i)
        {
            const editeng::SvxBorderLine* pLine = nullptr;
            pDoc->GetBorderLines(2, aChecks[i].mnRow, 0, nullptr, &pLine, nullptr, nullptr);
            CPPUNIT_ASSERT(pLine);
            CPPUNIT_ASSERT_EQUAL(toBorderName(aChecks[i].mnStyle),
                                 toBorderName(pLine->GetBorderLineStyle()));
            if (aChecks[i].mnWidth >= 0)
                CPPUNIT_ASSERT_EQUAL(aChecks[i].mnWidth, pLine->GetWidth());
        }
    }

    saveAndReload(sFormatType);
    ScDocument* pDoc = getScDoc();
    for (size_t i = 0; i < SAL_N_ELEMENTS(aChecks); ++i)
    {
        const editeng::SvxBorderLine* pLine = nullptr;
        pDoc->GetBorderLines(2, aChecks[i].mnRow, 0, nullptr, &pLine, nullptr, nullptr);
        CPPUNIT_ASSERT(pLine);
        CPPUNIT_ASSERT_EQUAL(toBorderName(aChecks[i].mnStyle),
                             toBorderName(pLine->GetBorderLineStyle()));
        if (aChecks[i].mnWidth >= 0)
            CPPUNIT_ASSERT_EQUAL(aChecks[i].mnWidth, pLine->GetWidth());
    }
}

void ScExportTest::testCellBordersXLS()
{
    createScDoc("xls/cell-borders.xls");
    testExcelCellBorders("MS Excel 97");
}

void ScExportTest::testCellBordersXLSX()
{
    createScDoc("xlsx/cell-borders.xlsx");
    testExcelCellBorders("Calc Office Open XML");
}

void ScExportTest::testBordersExchangeXLSX()
{
    // Document: sc/qa/unit/data/README.cellborders

    // short name for the table
    const SvxBorderLineStyle None = SvxBorderLineStyle::NONE;
    const SvxBorderLineStyle Solid = SvxBorderLineStyle::SOLID;
    const SvxBorderLineStyle Dotted = SvxBorderLineStyle::DOTTED;
    const SvxBorderLineStyle Dashed = SvxBorderLineStyle::DASHED;
    const SvxBorderLineStyle FineDash = SvxBorderLineStyle::FINE_DASHED;
    const SvxBorderLineStyle DashDot = SvxBorderLineStyle::DASH_DOT;
    const SvxBorderLineStyle DashDoDo = SvxBorderLineStyle::DASH_DOT_DOT;
    const SvxBorderLineStyle DoubThin = SvxBorderLineStyle::DOUBLE_THIN;

    const size_t nMaxCol = 18;
    const size_t nMaxRow = 7;

    static struct
    {
        SvxBorderLineStyle BorderStyleTop, BorderStyleBottom;
        tools::Long WidthTop, WidthBottom;
    } aCheckBorderWidth[nMaxCol][nMaxRow]
        = { /* Width */
            /* 0,05 */ { { Solid, Solid, 1, 1 }, // SOLID
                         { Dotted, Dotted, 15, 15 }, // DOTTED
                         { Dotted, Dotted, 15, 15 }, // DASHED
                         { FineDash, FineDash, 15, 15 }, // FINE_DASHED
                         { FineDash, FineDash, 15, 15 }, // DASH_DOT
                         { FineDash, FineDash, 15, 15 }, // DASH_DOT_DOT
                         { None, None, 0, 0 } }, // DOUBLE_THIN
            /* 0,25 */
            { { Solid, Solid, 1, 1 },
              { Dotted, Dotted, 15, 15 },
              { Dotted, Dotted, 15, 15 },
              { FineDash, FineDash, 15, 15 },
              { FineDash, FineDash, 15, 15 },
              { FineDash, FineDash, 15, 15 },
              { None, None, 0, 0 } },
            /* 0,50 */
            { { Solid, Solid, 1, 1 },
              { Dotted, Dotted, 15, 15 },
              { Dotted, Dotted, 15, 15 },
              { FineDash, FineDash, 15, 15 },
              { FineDash, FineDash, 15, 15 },
              { FineDash, FineDash, 15, 15 },
              { None, None, 0, 0 } },
            /* 0,75 */
            { { Solid, Solid, 15, 15 },
              { Dotted, Dotted, 15, 15 },
              { FineDash, FineDash, 15, 15 },
              { FineDash, FineDash, 15, 15 },
              { DashDot, DashDot, 15, 15 },
              { DashDoDo, DashDoDo, 15, 15 },
              { DoubThin, DoubThin, 35, 35 } },
            /* 1,00 */
            { { Solid, Solid, 15, 15 },
              { Dotted, Dotted, 15, 15 },
              { FineDash, FineDash, 15, 15 },
              { FineDash, FineDash, 15, 15 },
              { DashDot, DashDot, 15, 15 },
              { DashDoDo, DashDoDo, 15, 15 },
              { DoubThin, DoubThin, 35, 35 } },
            /* 1,25 */
            { { Solid, Solid, 15, 15 },
              { Dotted, Dotted, 15, 15 },
              { FineDash, FineDash, 15, 15 },
              { FineDash, FineDash, 15, 15 },
              { DashDot, DashDot, 15, 15 },
              { DashDoDo, DashDoDo, 15, 15 },
              { DoubThin, DoubThin, 35, 35 } },
            /* 1,50 */
            { { Solid, Solid, 15, 15 },
              { Dotted, Dotted, 15, 15 },
              { FineDash, FineDash, 15, 15 },
              { FineDash, FineDash, 15, 15 },
              { DashDot, DashDot, 15, 15 },
              { DashDoDo, DashDoDo, 15, 15 },
              { DoubThin, DoubThin, 35, 35 } },

            /* 1,75 */
            { { Solid, Solid, 35, 35 },
              { FineDash, FineDash, 35, 35 },
              { Dashed, Dashed, 35, 35 },
              { FineDash, FineDash, 35, 35 },
              { DashDot, DashDot, 35, 35 },
              { DashDoDo, DashDoDo, 35, 35 },
              { DoubThin, DoubThin, 35, 35 } },
            /* 2,00 */
            { { Solid, Solid, 35, 35 },
              { FineDash, FineDash, 35, 35 },
              { Dashed, Dashed, 35, 35 },
              { FineDash, FineDash, 35, 35 },
              { DashDot, DashDot, 35, 35 },
              { DashDoDo, DashDoDo, 35, 35 },
              { DoubThin, DoubThin, 35, 35 } },
            /* 2,25 */
            { { Solid, Solid, 35, 35 },
              { FineDash, FineDash, 35, 35 },
              { Dashed, Dashed, 35, 35 },
              { FineDash, FineDash, 35, 35 },
              { DashDot, DashDot, 35, 35 },
              { DashDoDo, DashDoDo, 35, 35 },
              { DoubThin, DoubThin, 35, 35 } },

            /* 2,50 */
            { { Solid, Solid, 50, 50 },
              { FineDash, FineDash, 35, 35 },
              { Dashed, Dashed, 35, 35 },
              { FineDash, FineDash, 35, 35 },
              { DashDot, DashDot, 35, 35 },
              { DashDoDo, DashDoDo, 35, 35 },
              { DoubThin, DoubThin, 35, 35 } },
            /* 2,75 */
            { { Solid, Solid, 50, 50 },
              { FineDash, FineDash, 35, 35 },
              { Dashed, Dashed, 35, 35 },
              { FineDash, FineDash, 35, 35 },
              { DashDot, DashDot, 35, 35 },
              { DashDoDo, DashDoDo, 35, 35 },
              { DoubThin, DoubThin, 35, 35 } },
            /* 3,00 */
            { { Solid, Solid, 50, 50 },
              { FineDash, FineDash, 35, 35 },
              { Dashed, Dashed, 35, 35 },
              { FineDash, FineDash, 35, 35 },
              { DashDot, DashDot, 35, 35 },
              { DashDoDo, DashDoDo, 35, 35 },
              { DoubThin, DoubThin, 35, 35 } },
            /* 3,50 */
            { { Solid, Solid, 50, 50 },
              { FineDash, FineDash, 35, 35 },
              { Dashed, Dashed, 35, 35 },
              { FineDash, FineDash, 35, 35 },
              { DashDot, DashDot, 35, 35 },
              { DashDoDo, DashDoDo, 35, 35 },
              { DoubThin, DoubThin, 35, 35 } },
            /* 4,00 */
            { { Solid, Solid, 50, 50 },
              { FineDash, FineDash, 35, 35 },
              { Dashed, Dashed, 35, 35 },
              { FineDash, FineDash, 35, 35 },
              { DashDot, DashDot, 35, 35 },
              { DashDoDo, DashDoDo, 35, 35 },
              { DoubThin, DoubThin, 35, 35 } },
            /* 5,00 */
            { { Solid, Solid, 50, 50 },
              { FineDash, FineDash, 35, 35 },
              { Dashed, Dashed, 35, 35 },
              { FineDash, FineDash, 35, 35 },
              { DashDot, DashDot, 35, 35 },
              { DashDoDo, DashDoDo, 35, 35 },
              { DoubThin, DoubThin, 35, 35 } },
            /* 7,00 */
            { { Solid, Solid, 50, 50 },
              { FineDash, FineDash, 35, 35 },
              { Dashed, Dashed, 35, 35 },
              { FineDash, FineDash, 35, 35 },
              { DashDot, DashDot, 35, 35 },
              { DashDoDo, DashDoDo, 35, 35 },
              { DoubThin, DoubThin, 35, 35 } },
            /* 9,00 */
            { { Solid, Solid, 50, 50 },
              { FineDash, FineDash, 35, 35 },
              { Dashed, Dashed, 35, 35 },
              { FineDash, FineDash, 35, 35 },
              { DashDot, DashDot, 35, 35 },
              { DashDoDo, DashDoDo, 35, 35 },
              { DoubThin, DoubThin, 35, 35 } }
          };

    createScDoc("ods/test_borders_export.ods");

    saveAndReload("Calc Office Open XML");
    ScDocument* pDoc = getScDoc();

    for (size_t nCol = 0; nCol < nMaxCol; ++nCol)
    {
        for (size_t nRow = 0; nRow < nMaxRow; ++nRow)
        {
            const editeng::SvxBorderLine* pLineTop = nullptr;
            const editeng::SvxBorderLine* pLineBottom = nullptr;
            pDoc->GetBorderLines(nCol + 2, (nRow * 2) + 8, 0, nullptr, &pLineTop, nullptr,
                                 &pLineBottom);
            if ((nCol < 3) && (nRow == 6))
            { // in this range no lines since minimum size to create a double is 0.5
                CPPUNIT_ASSERT(!pLineTop);
                CPPUNIT_ASSERT(!pLineBottom);
                continue;
            }
            else
            {
                CPPUNIT_ASSERT(pLineTop);
                CPPUNIT_ASSERT(pLineBottom);
            }

            CPPUNIT_ASSERT_EQUAL_MESSAGE("Top Border-Line-Style wrong",
                                         aCheckBorderWidth[nCol][nRow].BorderStyleTop,
                                         pLineTop->GetBorderLineStyle());
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Bottom Border-Line-Style wrong",
                                         aCheckBorderWidth[nCol][nRow].BorderStyleBottom,
                                         pLineBottom->GetBorderLineStyle());
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Top Width-Line wrong",
                                         aCheckBorderWidth[nCol][nRow].WidthTop,
                                         pLineTop->GetWidth());
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Bottom Width-Line wrong",
                                         aCheckBorderWidth[nCol][nRow].WidthBottom,
                                         pLineBottom->GetWidth());
        }
    }
}

static OUString toString(const ScBigRange& rRange)
{
    OUStringBuffer aBuf;
    aBuf.append("(columns:");
    aBuf.append(rRange.aStart.Col());
    aBuf.append('-');
    aBuf.append(rRange.aEnd.Col());
    aBuf.append(";rows:");
    aBuf.append(rRange.aStart.Row());
    aBuf.append('-');
    aBuf.append(rRange.aEnd.Row());
    aBuf.append(";sheets:");
    aBuf.append(rRange.aStart.Tab());
    aBuf.append('-');
    aBuf.append(rRange.aEnd.Tab());
    aBuf.append(')');

    return aBuf.makeStringAndClear();
}

void ScExportTest::testTrackChangesSimpleXLSX()
{
    struct CheckItem
    {
        sal_uLong mnActionId;
        ScChangeActionType meType;

        sal_Int32 mnStartCol;
        sal_Int32 mnStartRow;
        sal_Int32 mnStartTab;
        sal_Int32 mnEndCol;
        sal_Int32 mnEndRow;
        sal_Int32 mnEndTab;

        bool mbRowInsertedAtBottom;
    };

    struct
    {
        bool checkRange(ScChangeActionType eType, const ScBigRange& rExpected,
                        const ScBigRange& rActual)
        {
            ScBigRange aExpected(rExpected), aActual(rActual);

            switch (eType)
            {
                case SC_CAT_INSERT_ROWS:
                {
                    // Ignore columns.
                    aExpected.aStart.SetCol(0);
                    aExpected.aEnd.SetCol(0);
                    aActual.aStart.SetCol(0);
                    aActual.aEnd.SetCol(0);
                }
                break;
                default:;
            }

            return aExpected == aActual;
        }

        bool check(ScDocument& rDoc)
        {
            static const CheckItem aChecks[] = {
                { 1, SC_CAT_CONTENT, 1, 1, 0, 1, 1, 0, false },
                { 2, SC_CAT_INSERT_ROWS, 0, 2, 0, 0, 2, 0, true },
                { 3, SC_CAT_CONTENT, 1, 2, 0, 1, 2, 0, false },
                { 4, SC_CAT_INSERT_ROWS, 0, 3, 0, 0, 3, 0, true },
                { 5, SC_CAT_CONTENT, 1, 3, 0, 1, 3, 0, false },
                { 6, SC_CAT_INSERT_ROWS, 0, 4, 0, 0, 4, 0, true },
                { 7, SC_CAT_CONTENT, 1, 4, 0, 1, 4, 0, false },
                { 8, SC_CAT_INSERT_ROWS, 0, 5, 0, 0, 5, 0, true },
                { 9, SC_CAT_CONTENT, 1, 5, 0, 1, 5, 0, false },
                { 10, SC_CAT_INSERT_ROWS, 0, 6, 0, 0, 6, 0, true },
                { 11, SC_CAT_CONTENT, 1, 6, 0, 1, 6, 0, false },
                { 12, SC_CAT_INSERT_ROWS, 0, 7, 0, 0, 7, 0, true },
                { 13, SC_CAT_CONTENT, 1, 7, 0, 1, 7, 0, false },
            };

            ScChangeTrack* pCT = rDoc.GetChangeTrack();
            if (!pCT)
            {
                cerr << "Change track instance doesn't exist." << endl;
                return false;
            }

            sal_uLong nActionMax = pCT->GetActionMax();
            if (nActionMax != 13)
            {
                cerr << "Unexpected highest action ID value." << endl;
                return false;
            }

            for (size_t i = 0; i < SAL_N_ELEMENTS(aChecks); ++i)
            {
                sal_uInt16 nActId = aChecks[i].mnActionId;
                const ScChangeAction* pAction = pCT->GetAction(nActId);
                if (!pAction)
                {
                    cerr << "No action for action number " << nActId << " found." << endl;
                    return false;
                }

                if (pAction->GetType() != aChecks[i].meType)
                {
                    cerr << "Unexpected action type for action number " << nActId << "." << endl;
                    return false;
                }

                const ScBigRange& rRange = pAction->GetBigRange();
                ScBigRange aCheck(aChecks[i].mnStartCol, aChecks[i].mnStartRow,
                                  aChecks[i].mnStartTab, aChecks[i].mnEndCol, aChecks[i].mnEndRow,
                                  aChecks[i].mnEndTab);

                if (!checkRange(pAction->GetType(), aCheck, rRange))
                {
                    cerr << "Unexpected range for action number " << nActId
                         << ": expected=" << toString(aCheck) << " actual=" << toString(rRange)
                         << endl;
                    return false;
                }

                switch (pAction->GetType())
                {
                    case SC_CAT_INSERT_ROWS:
                    {
                        const ScChangeActionIns* p = static_cast<const ScChangeActionIns*>(pAction);
                        if (p->IsEndOfList() != aChecks[i].mbRowInsertedAtBottom)
                        {
                            cerr << "Unexpected end-of-list flag for action number " << nActId
                                 << "." << endl;
                            return false;
                        }
                    }
                    break;
                    default:;
                }
            }

            return true;
        }

        bool checkRevisionUserAndTime(ScDocument& rDoc, std::u16string_view rOwnerName)
        {
            ScChangeTrack* pCT = rDoc.GetChangeTrack();
            if (!pCT)
            {
                cerr << "Change track instance doesn't exist." << endl;
                return false;
            }

            ScChangeAction* pAction = pCT->GetLast();
            if (pAction->GetUser() != "Kohei Yoshida")
            {
                cerr << "Wrong user name." << endl;
                return false;
            }

            DateTime aDT = pAction->GetDateTime();
            if (aDT.GetYear() != 2014 || aDT.GetMonth() != 7 || aDT.GetDay() != 11)
            {
                cerr << "Wrong time stamp." << endl;
                return false;
            }

            // Insert a new record to make sure the user and date-time are correct.
            rDoc.SetString(ScAddress(1, 8, 0), "New String");
            ScCellValue aEmpty;
            pCT->AppendContent(ScAddress(1, 8, 0), aEmpty);
            pAction = pCT->GetLast();
            if (!pAction)
            {
                cerr << "Failed to retrieve last revision." << endl;
                return false;
            }

            if (rOwnerName != pAction->GetUser())
            {
                cerr << "Wrong user name." << endl;
                return false;
            }

            DateTime aDTNew = pAction->GetDateTime();
            if (aDTNew <= aDT)
            {
                cerr << "Time stamp of the new revision should be more recent than that of the "
                        "last revision."
                     << endl;
                return false;
            }

            return true;
        }

    } aTest;

    SvtUserOptions& rUserOpt = SC_MOD()->GetUserOptions();
    rUserOpt.SetToken(UserOptToken::FirstName, "Export");
    rUserOpt.SetToken(UserOptToken::LastName, "Test");

    OUString aOwnerName = rUserOpt.GetFirstName() + " " + rUserOpt.GetLastName();

    // First, test the xls variant.

    createScDoc("xls/track-changes/simple-cell-changes.xls");
    ScDocument* pDoc = getScDoc();
    bool bGood = aTest.check(*pDoc);
    CPPUNIT_ASSERT_MESSAGE("Initial check failed (xls).", bGood);

    saveAndReload("MS Excel 97");
    pDoc = getScDoc();
    bGood = aTest.check(*pDoc);
    CPPUNIT_ASSERT_MESSAGE("Check after reload failed (xls).", bGood);

    // fdo#81445 : Check the blank value string to make sure it's "<empty>".
    ScChangeTrack* pCT = pDoc->GetChangeTrack();
    CPPUNIT_ASSERT(pCT);
    ScChangeAction* pAction = pCT->GetAction(1);
    CPPUNIT_ASSERT(pAction);
    OUString aDesc = pAction->GetDescription(*pDoc);
    CPPUNIT_ASSERT_EQUAL(OUString("Cell B2 changed from '<empty>' to '1'"), aDesc);

    pDoc = getScDoc();
    bGood = aTest.checkRevisionUserAndTime(*pDoc, aOwnerName);
    CPPUNIT_ASSERT_MESSAGE("Check revision and time failed after reload (xls).", bGood);

    // Now, test the xlsx variant the same way.

    createScDoc("xlsx/track-changes/simple-cell-changes.xlsx");
    pDoc = getScDoc();
    aTest.check(*pDoc);
    CPPUNIT_ASSERT_MESSAGE("Initial check failed (xlsx).", bGood);

    saveAndReload("Calc Office Open XML");
    pDoc = getScDoc();
    bGood = aTest.check(*pDoc);
    CPPUNIT_ASSERT_MESSAGE("Check after reload failed (xlsx).", bGood);

    bGood = aTest.checkRevisionUserAndTime(*pDoc, aOwnerName);
    CPPUNIT_ASSERT_MESSAGE("Check revision and time failed after reload (xlsx).", bGood);
}

void ScExportTest::testSheetTabColorsXLSX()
{
    struct
    {
        bool checkContent(ScDocument& rDoc)
        {
            std::vector<OUString> aTabNames = rDoc.GetAllTableNames();

            // green, red, blue, yellow (from left to right).
            if (aTabNames.size() != 4)
            {
                cerr << "There should be exactly 4 sheets." << endl;
                return false;
            }

            const char* pNames[] = { "Green", "Red", "Blue", "Yellow" };
            for (size_t i = 0; i < SAL_N_ELEMENTS(pNames); ++i)
            {
                OUString aExpected = OUString::createFromAscii(pNames[i]);
                if (aExpected != aTabNames[i])
                {
                    cerr << "incorrect sheet name: expected='" << aExpected << "', actual='"
                         << aTabNames[i] << "'" << endl;
                    return false;
                }
            }

            static const Color aXclColors[] = {
                0x0000B050, // green
                0x00FF0000, // red
                0x000070C0, // blue
                0x00FFFF00, // yellow
            };

            for (size_t i = 0; i < SAL_N_ELEMENTS(aXclColors); ++i)
            {
                if (aXclColors[i] != rDoc.GetTabBgColor(i))
                {
                    cerr << "wrong sheet color for sheet " << i << endl;
                    return false;
                }
            }

            return true;
        }

    } aTest;

    createScDoc("xlsx/sheet-tab-color.xlsx");
    {
        ScDocument* pDoc = getScDoc();
        bool bRes = aTest.checkContent(*pDoc);
        CPPUNIT_ASSERT_MESSAGE("Failed on the initial content check.", bRes);
    }

    saveAndReload("Calc Office Open XML");
    ScDocument* pDoc = getScDoc();
    bool bRes = aTest.checkContent(*pDoc);
    CPPUNIT_ASSERT_MESSAGE("Failed on the content check after reload.", bRes);
}

void ScExportTest::testTdf133487()
{
    createScDoc("fods/shapes_foreground_background.fods");

    save("calc8");
    xmlDocUniquePtr pXmlDoc = parseExport("content.xml");
    CPPUNIT_ASSERT(pXmlDoc);

    // shape in background has lowest index
    assertXPath(pXmlDoc,
                "/office:document-content/office:body/office:spreadsheet/table:table[1]/"
                "table:table-row[1]/table:table-cell[1]/draw:custom-shape",
                "z-index", "0");
    assertXPath(pXmlDoc,
                "/office:document-content/office:body/office:spreadsheet/table:table[1]/"
                "table:table-row[1]/table:table-cell[1]/draw:custom-shape"
                "/attribute::table:table-background",
                1);
    assertXPath(pXmlDoc,
                "/office:document-content/office:body/office:spreadsheet/table:table[1]/"
                "table:table-row[1]/table:table-cell[1]/draw:custom-shape",
                "table-background", "true");
    // shape in foreground, previously index 1
    assertXPath(pXmlDoc,
                "/office:document-content/office:body/office:spreadsheet/table:table[1]/"
                "table:table-row[1]/table:table-cell[2]/draw:custom-shape",
                "z-index", "2");
    // attribute is only written for value "true"
    assertXPath(pXmlDoc,
                "/office:document-content/office:body/office:spreadsheet/table:table[1]/"
                "table:table-row[1]/table:table-cell[2]/draw:custom-shape"
                "/attribute::table:table-background",
                0);
    // shape in foreground, previously index 0
    assertXPath(pXmlDoc,
                "/office:document-content/office:body/office:spreadsheet/table:table[1]/"
                "table:table-row[3]/table:table-cell[1]/draw:custom-shape",
                "z-index", "1");
    // attribute is only written for value "true"
    assertXPath(pXmlDoc,
                "/office:document-content/office:body/office:spreadsheet/table:table[1]/"
                "table:table-row[3]/table:table-cell[1]/draw:custom-shape"
                "/attribute::table:table-background",
                0);
    // shape in foreground, previously index 4
    assertXPath(pXmlDoc,
                "/office:document-content/office:body/office:spreadsheet/table:table[1]/"
                "table:shapes/draw:custom-shape",
                "z-index", "3");
    // attribute is only written for value "true"
    assertXPath(pXmlDoc,
                "/office:document-content/office:body/office:spreadsheet/table:table[1]/"
                "table:shapes/draw:custom-shape"
                "/attribute::table:table-background",
                0);
    // form control, previously index 3
    assertXPath(pXmlDoc,
                "/office:document-content/office:body/office:spreadsheet/table:table[1]/"
                "table:shapes/draw:control",
                "z-index", "4");
    // attribute is only written for value "true"
    assertXPath(pXmlDoc,
                "/office:document-content/office:body/office:spreadsheet/table:table[1]/"
                "table:shapes/draw:control"
                "/attribute::table:table-background",
                0);
}

void ScExportTest::testSharedFormulaExportXLS()
{
    struct
    {
        bool checkContent(ScDocument& rDoc)
        {
            formula::FormulaGrammar::Grammar eGram = formula::FormulaGrammar::GRAM_ENGLISH_XL_R1C1;
            FormulaGrammarSwitch aFGSwitch(&rDoc, eGram);
            sc::TokenStringContext aCxt(rDoc, eGram);

            // Check the title row.

            OUString aActual = rDoc.GetString(0, 1, 0);
            OUString aExpected = "Response";
            if (aActual != aExpected)
            {
                cerr << "Wrong content in A2: expected='" << aExpected << "', actual='" << aActual
                     << "'" << endl;
                return false;
            }

            aActual = rDoc.GetString(1, 1, 0);
            aExpected = "Response";
            if (aActual != aExpected)
            {
                cerr << "Wrong content in B2: expected='" << aExpected << "', actual='" << aActual
                     << "'" << endl;
                return false;
            }

            // A3:A12 and B3:B12 are numbers from 1 to 10.
            for (SCROW i = 0; i <= 9; ++i)
            {
                double fExpected = i + 1.0;
                ScAddress aPos(0, i + 2, 0);
                double fActual = rDoc.GetValue(aPos);
                if (fExpected != fActual)
                {
                    cerr << "Wrong value in A" << (i + 2) << ": expected=" << fExpected
                         << ", actual=" << fActual << endl;
                    return false;
                }

                aPos.IncCol();
                ScFormulaCell* pFC = rDoc.GetFormulaCell(aPos);
                if (!pFC)
                {
                    cerr << "B" << (i + 2) << " should be a formula cell." << endl;
                    return false;
                }

                OUString aFormula = pFC->GetCode()->CreateString(aCxt, aPos);
                aExpected = "Coefficients!RC[-1]";
                if (aFormula != aExpected)
                {
                    cerr << "Wrong formula in B" << (i + 2) << ": expected='" << aExpected
                         << "', actual='" << aFormula << "'" << endl;
                    return false;
                }

                fActual = rDoc.GetValue(aPos);
                if (fExpected != fActual)
                {
                    cerr << "Wrong value in B" << (i + 2) << ": expected=" << fExpected
                         << ", actual=" << fActual << endl;
                    return false;
                }
            }

            return true;
        }

    } aTest;

    createScDoc("ods/shared-formula/3d-reference.ods");
    {
        // Check the content of the original.
        ScDocument* pDoc = getScDoc();
        bool bRes = aTest.checkContent(*pDoc);
        CPPUNIT_ASSERT_MESSAGE("Content check on the original document failed.", bRes);
    }

    saveAndReload("MS Excel 97");

    // Check the content of the reloaded. This should be identical.
    ScDocument* pDoc = getScDoc();
    bool bRes = aTest.checkContent(*pDoc);
    CPPUNIT_ASSERT_MESSAGE("Content check on the reloaded document failed.", bRes);
}

void ScExportTest::testSharedFormulaExportXLSX()
{
    struct
    {
        bool checkContent(ScDocument& rDoc)
        {
            SCTAB nTabCount = rDoc.GetTableCount();
            if (nTabCount != 2)
            {
                cerr << "Document should have exactly 2 sheets.  " << nTabCount << " found."
                     << endl;
                return false;
            }

            // Make sure the sheet tab colors are not set.
            for (SCROW i = 0; i <= 1; ++i)
            {
                Color aTabBgColor = rDoc.GetTabBgColor(i);
                if (aTabBgColor != COL_AUTO)
                {
                    cerr << "The tab color of Sheet " << (i + 1) << " should not be explicitly set."
                         << endl;
                    return false;
                }
            }

            // B2:B7 should show 1,2,3,4,5,6.
            double fExpected = 1.0;
            for (SCROW i = 1; i <= 6; ++i, ++fExpected)
            {
                ScAddress aPos(1, i, 0);
                double fVal = rDoc.GetValue(aPos);
                if (fVal != fExpected)
                {
                    cerr << "Wrong value in B" << (i + 1) << ": expected=" << fExpected
                         << ", actual=" << fVal << endl;
                    return false;
                }
            }

            // C2:C7 should show 10,20,...,60.
            fExpected = 10.0;
            for (SCROW i = 1; i <= 6; ++i, fExpected += 10.0)
            {
                ScAddress aPos(2, i, 0);
                double fVal = rDoc.GetValue(aPos);
                if (fVal != fExpected)
                {
                    cerr << "Wrong value in C" << (i + 1) << ": expected=" << fExpected
                         << ", actual=" << fVal << endl;
                    return false;
                }
            }

            // D2:D7 should show 1,2,...,6.
            fExpected = 1.0;
            for (SCROW i = 1; i <= 6; ++i, ++fExpected)
            {
                ScAddress aPos(3, i, 0);
                double fVal = rDoc.GetValue(aPos);
                if (fVal != fExpected)
                {
                    cerr << "Wrong value in D" << (i + 1) << ": expected=" << fExpected
                         << ", actual=" << fVal << endl;
                    return false;
                }
            }

            return true;
        }

    } aTest;

    createScDoc("xlsx/shared-formula/3d-reference.xlsx");
    {
        ScDocument* pDoc = getScDoc();
        bool bRes = aTest.checkContent(*pDoc);
        CPPUNIT_ASSERT_MESSAGE("Content check on the initial document failed.", bRes);

        pDoc->CalcAll(); // Recalculate to flush all cached results.
        bRes = aTest.checkContent(*pDoc);
        CPPUNIT_ASSERT_MESSAGE("Content check on the initial recalculated document failed.", bRes);
    }

    // Save and reload, and check the content again.
    saveAndReload("Calc Office Open XML");

    ScDocument* pDoc = getScDoc();
    pDoc->CalcAll(); // Recalculate to flush all cached results.

    bool bRes = aTest.checkContent(*pDoc);
    CPPUNIT_ASSERT_MESSAGE("Content check on the reloaded document failed.", bRes);
}

void ScExportTest::testSharedFormulaStringResultExportXLSX()
{
    struct
    {
        bool checkContent(ScDocument& rDoc)
        {
            {
                // B2:B7 should show A,B,...,F.
                const char* const expected[] = { "A", "B", "C", "D", "E", "F" };
                for (SCROW i = 0; i <= 5; ++i)
                {
                    ScAddress aPos(1, i + 1, 0);
                    OUString aStr = rDoc.GetString(aPos);
                    OUString aExpected = OUString::createFromAscii(expected[i]);
                    if (aStr != aExpected)
                    {
                        cerr << "Wrong value in B" << (i + 2) << ": expected='" << aExpected
                             << "', actual='" << aStr << "'" << endl;
                        return false;
                    }
                }
            }

            {
                // C2:C7 should show AA,BB,...,FF.
                const char* const expected[] = { "AA", "BB", "CC", "DD", "EE", "FF" };
                for (SCROW i = 0; i <= 5; ++i)
                {
                    ScAddress aPos(2, i + 1, 0);
                    OUString aStr = rDoc.GetString(aPos);
                    OUString aExpected = OUString::createFromAscii(expected[i]);
                    if (aStr != aExpected)
                    {
                        cerr << "Wrong value in C" << (i + 2) << ": expected='" << aExpected
                             << "', actual='" << aStr << "'" << endl;
                        return false;
                    }
                }
            }

            return true;
        }

    } aTest;

    createScDoc("xlsx/shared-formula/text-results.xlsx");
    {
        ScDocument* pDoc = getScDoc();

        // Check content without re-calculation, to test cached formula results.
        bool bRes = aTest.checkContent(*pDoc);
        CPPUNIT_ASSERT_MESSAGE("Content check on the initial document failed.", bRes);

        // Now, re-calculate and check the results.
        pDoc->CalcAll();
        bRes = aTest.checkContent(*pDoc);
        CPPUNIT_ASSERT_MESSAGE("Content check on the initial recalculated document failed.", bRes);
    }
    // Reload and check again.
    saveAndReload("Calc Office Open XML");
    ScDocument* pDoc = getScDoc();

    bool bRes = aTest.checkContent(*pDoc);
    CPPUNIT_ASSERT_MESSAGE("Content check on the reloaded document failed.", bRes);
}

void ScExportTest::testFunctionsExcel2010(const OUString& sFormatType)
{
    createScDoc("xlsx/functions-excel-2010.xlsx");

    saveAndReload(sFormatType);
    ScDocument* pDoc = getScDoc();
    pDoc->CalcAll(); // perform hard re-calculation.

    testFunctionsExcel2010_Impl(*pDoc);
}

void ScExportTest::testFunctionsExcel2010XLSX() { testFunctionsExcel2010("Calc Office Open XML"); }

void ScExportTest::testFunctionsExcel2010XLS() { testFunctionsExcel2010("MS Excel 97"); }

void ScExportTest::testCeilingFloor(const OUString& sFormatType)
{
    createScDoc("xlsx/ceiling-floor.xlsx");

    saveAndReload(sFormatType);
    ScDocument* pDoc = getScDoc();
    pDoc->CalcAll(); // perform hard re-calculation.

    testCeilingFloor_Impl(*pDoc);
}

void ScExportTest::testCeilingFloorXLSX() { testCeilingFloor("Calc Office Open XML"); }

void ScExportTest::testCeilingFloorODSToXLSX()
{
    // tdf#100011 - Cannot open sheet containing FLOOR/CEILING functions by MS Excel, after export to .xlsx
    createScDoc("ods/ceiling-floor.ods");

    save("Calc Office Open XML");
    xmlDocUniquePtr pSheet = parseExport("xl/workbook.xml");
    CPPUNIT_ASSERT(pSheet);

    // there shouldn't be any defined names during export of FLOOR and CEILING functions to .xlsx
    assertXPath(pSheet, "/x:workbook/x:definedNames", 0);
}

void ScExportTest::testCeilingFloorXLS() { testCeilingFloor("MS Excel 97"); }

void ScExportTest::testCeilingFloorODS() { testCeilingFloor("calc8"); }

void ScExportTest::testCustomXml()
{
    // Load document and export it to a temporary file
    createScDoc("xlsx/customxml.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pXmlDoc = parseExport("customXml/item1.xml");
    CPPUNIT_ASSERT(pXmlDoc);
    xmlDocUniquePtr pRelsDoc = parseExport("customXml/_rels/item1.xml.rels");
    CPPUNIT_ASSERT(pRelsDoc);

    // Check there is a relation to itemProps1.xml.
    assertXPath(pRelsDoc, "/rels:Relationships/rels:Relationship", 1);
    assertXPath(pRelsDoc, "/rels:Relationships/rels:Relationship[@Id='rId1']", "Target",
                "itemProps1.xml");

    std::unique_ptr<SvStream> pStream = parseExportStream(maTempFile.GetURL(), "ddp/ddpfile.xen");
    CPPUNIT_ASSERT(pStream);
}

#ifdef _WIN32
static sal_Unicode lcl_getWindowsDrive(const OUString& aURL)
{
    static const sal_Int32 nMinLen = strlen("file:///X:/");
    if (aURL.getLength() <= nMinLen)
        return 0;
    const OUString aUrlStart = aURL.copy(0, nMinLen);
    return (aUrlStart.startsWith("file:///") && aUrlStart.endsWith(":/")) ? aUrlStart[8] : 0;
}
#endif

void ScExportTest::testRelativePathsODS()
{
    createScDoc("ods/fdo79305.ods");

    save("calc8");
    xmlDocUniquePtr pDoc = parseExport("content.xml");
    CPPUNIT_ASSERT(pDoc);
    OUString aURL = getXPath(pDoc,
                             "/office:document-content/office:body/office:spreadsheet/table:table/"
                             "table:table-row[2]/table:table-cell[2]/text:p/text:a",
                             "href");
#ifdef _WIN32
    // if the exported document is not on the same drive then the linked document,
    // there is no way to get a relative URL for the link, because ../X:/ is undefined.
    if (!aURL.startsWith(".."))
    {
        sal_Unicode aDocDrive = lcl_getWindowsDrive(maTempFile.GetURL());
        sal_Unicode aLinkDrive = lcl_getWindowsDrive(aURL);
        CPPUNIT_ASSERT_MESSAGE("document on the same drive but no relative link!", aDocDrive != 0);
        CPPUNIT_ASSERT_MESSAGE("document on the same drive but no relative link!", aLinkDrive != 0);
        CPPUNIT_ASSERT_MESSAGE("document on the same drive but no relative link!",
                               aDocDrive != aLinkDrive);
        return;
    }
#endif
    // make sure that the URL is relative
    CPPUNIT_ASSERT(aURL.startsWith(".."));
}

namespace
{
void testSheetProtection_Impl(ScDocument& rDoc)
{
    CPPUNIT_ASSERT(rDoc.IsTabProtected(0));
    const ScTableProtection* pTabProtection = rDoc.GetTabProtection(0);
    CPPUNIT_ASSERT(pTabProtection);
    CPPUNIT_ASSERT(pTabProtection->isOptionEnabled(ScTableProtection::SELECT_UNLOCKED_CELLS));
    CPPUNIT_ASSERT(!pTabProtection->isOptionEnabled(ScTableProtection::SELECT_LOCKED_CELLS));
}
}

void ScExportTest::testSheetProtectionODS()
{
    createScDoc("ods/sheet-protection.ods");

    ScDocument* pDoc = getScDoc();

    testSheetProtection_Impl(*pDoc);

    saveAndReload("calc8");

    pDoc = getScDoc();

    testSheetProtection_Impl(*pDoc);
}

void ScExportTest::testFunctionsExcel2010ODS()
{
    //testFunctionsExcel2010("calc8");
}

void ScExportTest::testSwappedOutImageExport()
{
    std::vector<OUString> aFilterNames{ "calc8", "MS Excel 97", "Calc Office Open XML" };

    // Set cache size to a very small value to make sure one of the images is swapped out
    std::shared_ptr<comphelper::ConfigurationChanges> xBatch(
        comphelper::ConfigurationChanges::create());
    officecfg::Office::Common::Cache::GraphicManager::TotalCacheSize::set(sal_Int32(1), xBatch);
    xBatch->commit();

    for (size_t i = 0; i < aFilterNames.size(); ++i)
    {
        // Check whether the export code swaps in the image which was swapped out before.
        createScDoc("ods/document_with_two_images.ods");

        const OString sFailedMessage
            = OString::Concat("Failed on filter: ") + aFilterNames[i].toUtf8();

        // Export the document and import again for a check
        saveAndReload(aFilterNames[i]);

        // Check whether graphic exported well after it was swapped out
        uno::Reference<sheet::XSpreadsheetDocument> xDoc(mxComponent, UNO_QUERY_THROW);
        uno::Reference<container::XIndexAccess> xIA(xDoc->getSheets(), UNO_QUERY_THROW);
        uno::Reference<drawing::XDrawPageSupplier> xDrawPageSupplier(xIA->getByIndex(0),
                                                                     UNO_QUERY_THROW);
        uno::Reference<container::XIndexAccess> xDraws(xDrawPageSupplier->getDrawPage(),
                                                       UNO_QUERY_THROW);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(sFailedMessage.getStr(), static_cast<sal_Int32>(2),
                                     xDraws->getCount());

        uno::Reference<drawing::XShape> xImage(xDraws->getByIndex(0), uno::UNO_QUERY);
        uno::Reference<beans::XPropertySet> XPropSet(xImage, uno::UNO_QUERY_THROW);

        // Check Graphic, Size
        {
            uno::Reference<graphic::XGraphic> xGraphic;
            XPropSet->getPropertyValue("Graphic") >>= xGraphic;
            CPPUNIT_ASSERT_MESSAGE(sFailedMessage.getStr(), xGraphic.is());
            CPPUNIT_ASSERT_MESSAGE(sFailedMessage.getStr(),
                                   xGraphic->getType() != graphic::GraphicType::EMPTY);
            uno::Reference<awt::XBitmap> xBitmap(xGraphic, uno::UNO_QUERY);
            CPPUNIT_ASSERT_MESSAGE(sFailedMessage.getStr(), xBitmap.is());
            CPPUNIT_ASSERT_EQUAL_MESSAGE(sFailedMessage.getStr(), static_cast<sal_Int32>(610),
                                         xBitmap->getSize().Width);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(sFailedMessage.getStr(), static_cast<sal_Int32>(381),
                                         xBitmap->getSize().Height);
        }
        // Second Image
        xImage.set(xDraws->getByIndex(1), uno::UNO_QUERY);
        XPropSet.set(xImage, uno::UNO_QUERY_THROW);

        // Check Graphic, Size
        {
            uno::Reference<graphic::XGraphic> xGraphic;
            XPropSet->getPropertyValue("Graphic") >>= xGraphic;
            CPPUNIT_ASSERT_MESSAGE(sFailedMessage.getStr(), xGraphic.is());
            CPPUNIT_ASSERT_MESSAGE(sFailedMessage.getStr(),
                                   xGraphic->getType() != graphic::GraphicType::EMPTY);
            uno::Reference<awt::XBitmap> xBitmap(xGraphic, uno::UNO_QUERY);
            CPPUNIT_ASSERT_MESSAGE(sFailedMessage.getStr(), xBitmap.is());
            CPPUNIT_ASSERT_EQUAL_MESSAGE(sFailedMessage.getStr(), static_cast<sal_Int32>(900),
                                         xBitmap->getSize().Width);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(sFailedMessage.getStr(), static_cast<sal_Int32>(600),
                                         xBitmap->getSize().Height);
        }
    }
}

void ScExportTest::testSupBookVirtualPathXLS()
{
    createScDoc("xls/external-ref.xls");

    saveAndReload("MS Excel 97");

    ScDocument* pDoc = getScDoc();

    OUString aFormula = pDoc->GetFormula(0, 0, 0);
#ifdef _WIN32
    aFormula = OUString::Concat(aFormula.subView(0, 9)) + aFormula.subView(12);
    // strip drive letter, e.g. 'C:/'
#endif
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Wrong SupBook VirtualPath URL",
        OUString("='file:///home/timar/Documents/external.xls'#$Sheet1.A1"), aFormula);
}

void ScExportTest::testLinkedGraphicRT()
{
    // Problem was with linked images
    std::vector<OUString> aFilterNames{ "calc8", "MS Excel 97", "Calc Office Open XML" };

    for (size_t i = 0; i < aFilterNames.size(); ++i)
    {
        // Load the original file with one image
        createScDoc("ods/document_with_linked_graphic.ods");
        const OString sFailedMessage
            = OString::Concat("Failed on filter: ") + aFilterNames[i].toUtf8();

        // Export the document and import again for a check
        saveAndReload(aFilterNames[i]);

        // Check whether graphic imported well after export
        ScDocument* pDoc = getScDoc();
        ScDrawLayer* pDrawLayer = pDoc->GetDrawLayer();
        CPPUNIT_ASSERT_MESSAGE(sFailedMessage.getStr(), pDrawLayer != nullptr);
        const SdrPage* pPage = pDrawLayer->GetPage(0);
        CPPUNIT_ASSERT_MESSAGE(sFailedMessage.getStr(), pPage != nullptr);
        SdrGrafObj* pObject = dynamic_cast<SdrGrafObj*>(pPage->GetObj(0));
        CPPUNIT_ASSERT_MESSAGE(sFailedMessage.getStr(), pObject != nullptr);
        if (aFilterNames[i] == "Calc Office Open XML")
        {
            // FIXME: tdf#152036
            CPPUNIT_ASSERT_MESSAGE(sFailedMessage.getStr(), !pObject->IsLinkedGraphic());
        }
        else
            CPPUNIT_ASSERT_MESSAGE(sFailedMessage.getStr(), pObject->IsLinkedGraphic());

        const GraphicObject& rGraphicObj = pObject->GetGraphicObject(true);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(sFailedMessage.getStr(), int(GraphicType::Bitmap),
                                     int(rGraphicObj.GetGraphic().GetType()));
        CPPUNIT_ASSERT_EQUAL_MESSAGE(sFailedMessage.getStr(), sal_uLong(864900),
                                     rGraphicObj.GetGraphic().GetSizeBytes());
    }
}

void ScExportTest::testImageWithSpecialID()
{
    std::vector<OUString> aFilterNames{ "calc8", "MS Excel 97", "Calc Office Open XML" };

    // Trigger swap out mechanism to test swapped state factor too.
    std::shared_ptr<comphelper::ConfigurationChanges> batch(
        comphelper::ConfigurationChanges::create());
    officecfg::Office::Common::Cache::GraphicManager::TotalCacheSize::set(sal_Int32(1), batch);
    batch->commit();

    for (size_t i = 0; i < aFilterNames.size(); ++i)
    {
        createScDoc("ods/images_with_special_IDs.ods");

        const OString sFailedMessage
            = OString::Concat("Failed on filter: ") + aFilterNames[i].toUtf8();

        // Export the document and import again for a check
        saveAndReload(aFilterNames[i]);

        // Check whether graphic was exported well
        uno::Reference<sheet::XSpreadsheetDocument> xDoc(mxComponent, UNO_QUERY_THROW);
        uno::Reference<container::XIndexAccess> xIA(xDoc->getSheets(), UNO_QUERY_THROW);
        uno::Reference<drawing::XDrawPageSupplier> xDrawPageSupplier(xIA->getByIndex(0),
                                                                     UNO_QUERY_THROW);
        uno::Reference<container::XIndexAccess> xDraws(xDrawPageSupplier->getDrawPage(),
                                                       UNO_QUERY_THROW);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(sFailedMessage.getStr(), static_cast<sal_Int32>(2),
                                     xDraws->getCount());

        uno::Reference<drawing::XShape> xImage(xDraws->getByIndex(0), uno::UNO_QUERY);
        uno::Reference<beans::XPropertySet> XPropSet(xImage, uno::UNO_QUERY_THROW);

        // Check Graphic, Size
        {
            uno::Reference<graphic::XGraphic> xGraphic;
            XPropSet->getPropertyValue("Graphic") >>= xGraphic;
            CPPUNIT_ASSERT_MESSAGE(sFailedMessage.getStr(), xGraphic.is());
            CPPUNIT_ASSERT_MESSAGE(sFailedMessage.getStr(),
                                   xGraphic->getType() != graphic::GraphicType::EMPTY);
            uno::Reference<awt::XBitmap> xBitmap(xGraphic, uno::UNO_QUERY);
            CPPUNIT_ASSERT_MESSAGE(sFailedMessage.getStr(), xBitmap.is());
            CPPUNIT_ASSERT_EQUAL_MESSAGE(sFailedMessage.getStr(), static_cast<sal_Int32>(610),
                                         xBitmap->getSize().Width);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(sFailedMessage.getStr(), static_cast<sal_Int32>(381),
                                         xBitmap->getSize().Height);
        }
        // Second Image
        xImage.set(xDraws->getByIndex(1), uno::UNO_QUERY);
        XPropSet.set(xImage, uno::UNO_QUERY_THROW);

        // Check Graphic, Size
        {
            uno::Reference<graphic::XGraphic> xGraphic;
            XPropSet->getPropertyValue("Graphic") >>= xGraphic;
            CPPUNIT_ASSERT_MESSAGE(sFailedMessage.getStr(), xGraphic.is());
            CPPUNIT_ASSERT_MESSAGE(sFailedMessage.getStr(),
                                   xGraphic->getType() != graphic::GraphicType::EMPTY);
            uno::Reference<awt::XBitmap> xBitmap(xGraphic, uno::UNO_QUERY);
            CPPUNIT_ASSERT_MESSAGE(sFailedMessage.getStr(), xBitmap.is());
            CPPUNIT_ASSERT_EQUAL_MESSAGE(sFailedMessage.getStr(), static_cast<sal_Int32>(900),
                                         xBitmap->getSize().Width);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(sFailedMessage.getStr(), static_cast<sal_Int32>(600),
                                         xBitmap->getSize().Height);
        }
    }
}

void ScExportTest::testAbsNamedRangeHTML()
{
    setImportFilterName("calc_HTML_WebQuery");
    createScDoc("html/numberformat.html");
    ScDocShell* pDocSh = getScDocShell();
    pDocSh->DoHardRecalc();

    //reset import filter
    setImportFilterName("calc8");
    saveAndReload("calc8");
    pDocSh = getScDocShell();
    pDocSh->DoHardRecalc();

    ScDocument* pDoc = getScDoc();
    ScRangeData* pRangeData = pDoc->GetRangeName()->findByUpperName(OUString("HTML_1"));
    ScSingleRefData* pRef = pRangeData->GetCode()->FirstToken()->GetSingleRef();
    // see tdf#119141 for the reason why this isn't Sheet1.HTML_1
    CPPUNIT_ASSERT_MESSAGE("HTML_1 is an absolute reference", !pRef->IsTabRel());
}

void ScExportTest::testTdf80149()
{
    createScDoc("csv/tdf80149.csv");
    ScDocShell* pDocSh = getScDocShell();
    pDocSh->DoHardRecalc();
    saveAndReload("Calc Office Open XML");
    pDocSh = getScDocShell();
    pDocSh->DoHardRecalc();

    ScDocument* pDoc = getScDoc();
    CPPUNIT_ASSERT_EQUAL(OUString("row 1"), pDoc->GetString(0, 0, 0));

    // Without the fix in place, this test would have failed with
    // - Expected: Character 0x16 is here ->><<--
    // - Actual  :
    CPPUNIT_ASSERT_EQUAL(OUString("Character 0x16 is here ->><<--"), pDoc->GetString(1, 0, 0));
    CPPUNIT_ASSERT_EQUAL(OUString("File opens in libre office, but can't be saved as xlsx"),
                         pDoc->GetString(2, 0, 0));
    CPPUNIT_ASSERT_EQUAL(OUString("row 2"), pDoc->GetString(0, 1, 0));
    CPPUNIT_ASSERT_EQUAL(OUString("Subsequent rows get truncated"), pDoc->GetString(1, 1, 0));
    CPPUNIT_ASSERT_EQUAL(OUString("This cell goes missing"), pDoc->GetString(2, 1, 0));
    CPPUNIT_ASSERT_EQUAL(OUString("row 3"), pDoc->GetString(0, 2, 0));
    CPPUNIT_ASSERT_EQUAL(OUString("Subsequent rows get truncated"), pDoc->GetString(1, 2, 0));
    CPPUNIT_ASSERT_EQUAL(OUString("This cell goes missing"), pDoc->GetString(2, 2, 0));
}

void ScExportTest::testSheetLocalRangeNameXLS()
{
    createScDoc("xls/named-ranges-local.xls");
    ScDocShell* pDocSh = getScDocShell();
    pDocSh->DoHardRecalc();
    saveAndReload("MS Excel 97");
    pDocSh = getScDocShell();
    pDocSh->DoHardRecalc();

    ScDocument* pDoc = getScDoc();
    ScRangeName* pRangeName = pDoc->GetRangeName(0);
    CPPUNIT_ASSERT(pRangeName);
    CPPUNIT_ASSERT_EQUAL(size_t(2), pRangeName->size());

    OUString aFormula = pDoc->GetFormula(3, 11, 0);
    CPPUNIT_ASSERT_EQUAL(OUString("=SUM(local_name2)"), aFormula);
    ASSERT_DOUBLES_EQUAL(14.0, pDoc->GetValue(3, 11, 0));

    aFormula = pDoc->GetFormula(6, 4, 0);
    CPPUNIT_ASSERT_EQUAL(OUString("=local_name1"), aFormula);
}

void ScExportTest::testRelativeNamedExpressionsXLS()
{
    createScDoc("ods/tdf113991_relativeNamedRanges.ods");
    ScDocShell* pDocSh = getScDocShell();
    pDocSh->DoHardRecalc();
    saveAndReload("MS Excel 97");
    pDocSh = getScDocShell();
    pDocSh->DoHardRecalc();

    ScDocument* pDoc = getScDoc();
    // Sheet1:G3
    ScAddress aPos(6, 2, 0);
    CPPUNIT_ASSERT_EQUAL(1.0, pDoc->GetValue(aPos));
    CPPUNIT_ASSERT_EQUAL(OUString("=single_cell_A3"),
                         pDoc->GetFormula(aPos.Col(), aPos.Row(), aPos.Tab()));
    // Sheet2:F6
    aPos = ScAddress(5, 5, 1);
    CPPUNIT_ASSERT_EQUAL(18.0, pDoc->GetValue(aPos));
    CPPUNIT_ASSERT_EQUAL(OUString("=SUM(test_conflict)"),
                         pDoc->GetFormula(aPos.Col(), aPos.Row(), aPos.Tab()));
    // Sheet2:H3
    aPos = ScAddress(7, 2, 1);
    CPPUNIT_ASSERT_EQUAL(10.0, pDoc->GetValue(aPos));
    CPPUNIT_ASSERT_EQUAL(OUString("=single_global_A3"),
                         pDoc->GetFormula(aPos.Col(), aPos.Row(), aPos.Tab()));
    // Sheet2:H6
    aPos = ScAddress(7, 5, 1);
    CPPUNIT_ASSERT_EQUAL(75.0, pDoc->GetValue(aPos));
    CPPUNIT_ASSERT_EQUAL(OUString("=SUM(A6:F6)"),
                         pDoc->GetFormula(aPos.Col(), aPos.Row(), aPos.Tab()));
}

void ScExportTest::testSheetTextBoxHyperlinkXLSX()
{
    createScDoc("xlsx/textbox-hyperlink.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/drawings/drawing1.xml");
    CPPUNIT_ASSERT(pDoc);

    assertXPath(
        pDoc,
        "/xdr:wsDr[1]/xdr:twoCellAnchor[1]/xdr:sp[1]/xdr:nvSpPr[1]/xdr:cNvPr[1]/a:hlinkClick[1]",
        1);
}

void ScExportTest::testFontSizeXLSX()
{
    createScDoc("xlsx/fontSize.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/drawings/drawing1.xml");
    CPPUNIT_ASSERT(pDoc);
    OUString fontSize = getXPath(
        pDoc, "/xdr:wsDr/xdr:twoCellAnchor/xdr:sp[1]/xdr:txBody/a:p[1]/a:r[1]/a:rPr", "sz");
    // make sure that the font size is 18
    CPPUNIT_ASSERT_EQUAL(OUString("1800"), fontSize);
}

void ScExportTest::testSheetCharacterKerningSpaceXLSX()
{
    createScDoc("xlsx/textbox-CharKerningSpace.xlsx");

    saveAndReload("Calc Office Open XML");

    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/drawings/drawing1.xml");
    CPPUNIT_ASSERT(pDoc);

    OUString CharKerningSpace = getXPath(
        pDoc, "/xdr:wsDr[1]/xdr:twoCellAnchor[1]/xdr:sp[1]/xdr:txBody[1]/a:p[1]/a:r[1]/a:rPr[1]",
        "spc");

    // make sure that the CharKerning is 1997.
    CPPUNIT_ASSERT_EQUAL(OUString("1997"), CharKerningSpace);
}

void ScExportTest::testSheetCondensedCharacterSpaceXLSX()
{
    createScDoc("xlsx/textbox-CondensedCharacterSpace.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/drawings/drawing1.xml");
    CPPUNIT_ASSERT(pDoc);

    OUString CondensedCharSpace = getXPath(
        pDoc, "/xdr:wsDr[1]/xdr:twoCellAnchor[1]/xdr:sp[1]/xdr:txBody[1]/a:p[1]/a:r[1]/a:rPr[1]",
        "spc");

    // make sure that the CondensedCharSpace is -1002.
    CPPUNIT_ASSERT_EQUAL(OUString("-1002"), CondensedCharSpace);
}

void ScExportTest::testTextUnderlineColorXLSX()
{
    createScDoc("xlsx/underlineColor.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/drawings/drawing1.xml");
    CPPUNIT_ASSERT(pDoc);
    // Make sure the underline type is double line
    assertXPath(pDoc, "/xdr:wsDr/xdr:twoCellAnchor[1]/xdr:sp[1]/xdr:txBody/a:p[1]/a:r[1]/a:rPr",
                "u", "dbl");

    assertXPath(pDoc, "/xdr:wsDr/xdr:twoCellAnchor[1]/xdr:sp[1]/xdr:txBody/a:p[1]/a:r[1]/a:rPr",
                "b", "1");
    // Make sure that the underline color is RED
    assertXPath(pDoc,
                "/xdr:wsDr/xdr:twoCellAnchor[1]/xdr:sp[1]/xdr:txBody/a:p[1]/a:r[1]/a:rPr/a:uFill/"
                "a:solidFill/a:srgbClr",
                "val", "ff0000");

    // Make sure the underline type is drawn with heavy line
    assertXPath(pDoc, "/xdr:wsDr/xdr:twoCellAnchor[2]/xdr:sp[1]/xdr:txBody/a:p[1]/a:r[1]/a:rPr",
                "u", "heavy");
    // tdf#104219 Make sure that uFill is not existing and uFillTx is set.
    // It mean that color is automatic, should be the same color as the text.
    assertXPath(
        pDoc, "/xdr:wsDr/xdr:twoCellAnchor[2]/xdr:sp[1]/xdr:txBody/a:p[1]/a:r[1]/a:rPr/a:uFill", 0);
    assertXPath(pDoc,
                "/xdr:wsDr/xdr:twoCellAnchor[2]/xdr:sp[1]/xdr:txBody/a:p[1]/a:r[1]/a:rPr/a:uFillTx",
                1);
}

void ScExportTest::testSheetRunParagraphPropertyXLSX()
{
    createScDoc("xlsx/TextColor.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/sharedStrings.xml");
    CPPUNIT_ASSERT(pDoc);

    OUString aColor = getXPath(pDoc, "/x:sst/x:si/x:r[1]/x:rPr[1]/x:color", "rgb");
    CPPUNIT_ASSERT_EQUAL(OUString("FFFF0000"), aColor);
}

void ScExportTest::testPreserveTextWhitespaceXLSX()
{
    createScDoc("xlsx/preserve-whitespace.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/sharedStrings.xml");
    CPPUNIT_ASSERT(pDoc);
    assertXPath(pDoc, "/x:sst/x:si/x:t", "space", "preserve");
}

void ScExportTest::testPreserveTextWhitespace2XLSX()
{
    createScDoc("xlsx/preserve_space.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/sharedStrings.xml");
    CPPUNIT_ASSERT(pDoc);
    assertXPath(pDoc, "/x:sst/x:si[1]/x:t", "space", "preserve");
    assertXPath(pDoc, "/x:sst/x:si[2]/x:r[1]/x:t", "space", "preserve");
    assertXPath(pDoc, "/x:sst/x:si[2]/x:r[2]/x:t", "space", "preserve");
}

void ScExportTest::testHiddenShapeXLS()
{
    createScDoc("xls/hiddenShape.xls");

    ScDocument* pDoc = getScDoc();
    CPPUNIT_ASSERT(pDoc->GetTableCount() > 0);
    ScDrawLayer* pDrawLayer = pDoc->GetDrawLayer();
    SdrPage* pPage = pDrawLayer->GetPage(0);
    CPPUNIT_ASSERT(pPage);
    SdrObject* pObj = pPage->GetObj(0);
    CPPUNIT_ASSERT(pObj);
    CPPUNIT_ASSERT_MESSAGE("Drawing object should not be visible.", !pObj->IsVisible());
    CPPUNIT_ASSERT_MESSAGE("Drawing object should not be printable.", !pObj->IsPrintable());
}

void ScExportTest::testHiddenShapeXLSX()
{
    createScDoc("xlsx/hiddenShape.xlsx");

    ScDocument* pDoc = getScDoc();
    CPPUNIT_ASSERT(pDoc->GetTableCount() > 0);
    ScDrawLayer* pDrawLayer = pDoc->GetDrawLayer();
    SdrPage* pPage = pDrawLayer->GetPage(0);
    CPPUNIT_ASSERT(pPage);
    SdrObject* pObj = pPage->GetObj(0);
    CPPUNIT_ASSERT(pObj);
    CPPUNIT_ASSERT_MESSAGE("Drawing object should not be visible.", !pObj->IsVisible());
    CPPUNIT_ASSERT_MESSAGE("Drawing object should not be printable.", !pObj->IsPrintable());

    save("Calc Office Open XML");
    xmlDocUniquePtr pDocXml = parseExport("xl/drawings/drawing1.xml");
    CPPUNIT_ASSERT(pDocXml);
    assertXPath(pDocXml, "/xdr:wsDr/xdr:twoCellAnchor/xdr:sp[1]/xdr:nvSpPr/xdr:cNvPr", "hidden",
                "1");
}

void ScExportTest::testShapeAutofitXLSX()
{
    createScDoc("xlsx/testShapeAutofit.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/drawings/drawing1.xml");
    CPPUNIT_ASSERT(pDoc);

    // TextAutoGrowHeight --> "Fit height to text" / "Resize shape to fit text" --> true
    assertXPath(pDoc, "/xdr:wsDr/xdr:twoCellAnchor[1]/xdr:sp/xdr:txBody/a:bodyPr/a:spAutoFit", 1);
    // TextAutoGrowHeight --> "Fit height to text" / "Resize shape to fit text" --> false
    assertXPath(pDoc, "/xdr:wsDr/xdr:twoCellAnchor[2]/xdr:sp/xdr:txBody/a:bodyPr/a:noAutofit", 1);
}

void ScExportTest::testHyperlinkXLSX()
{
    createScDoc("xlsx/hyperlink.xlsx");

    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/drawings/_rels/drawing1.xml.rels");
    CPPUNIT_ASSERT(pDoc);
    assertXPath(pDoc, "/rels:Relationships/rels:Relationship", "Target", "#Sheet2!A1");
}

void ScExportTest::testMoveCellAnchoredShapesODS()
{
    createScDoc("ods/move-cell-anchored-shapes.ods");

    // There are two cell-anchored objects on the first sheet.
    ScDocument* pDoc = getScDoc();

    CPPUNIT_ASSERT_MESSAGE("There should be at least one sheet.", pDoc->GetTableCount() > 0);

    ScDrawLayer* pDrawLayer = pDoc->GetDrawLayer();
    SdrPage* pPage = pDrawLayer->GetPage(0);
    CPPUNIT_ASSERT_MESSAGE("draw page for sheet 1 should exist.", pPage);
    SdrObject* pObj = pPage->GetObj(0);
    CPPUNIT_ASSERT_MESSAGE("Failed to get drawing object.", pObj);

    // Check cell anchor state
    ScAnchorType oldType = ScDrawLayer::GetAnchorType(*pObj);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to get anchor type", SCA_CELL_RESIZE, oldType);

    // Get anchor data
    ScDrawObjData* pData = ScDrawLayer::GetObjData(pObj);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve user data for this object.", pData);
    CPPUNIT_ASSERT_MESSAGE("Bounding rectangle should have been calculated upon import.",
                           !pData->getShapeRect().IsEmpty());

    ScAddress aDataStart = pData->maStart;
    ScAddress aDataEnd = pData->maEnd;

    // Get non rotated anchor data
    ScDrawObjData* pNData = ScDrawLayer::GetNonRotatedObjData(pObj);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve non rotated user data for this object.", pNData);
    CPPUNIT_ASSERT_MESSAGE("Bounding rectangle should have been calculated upon import.",
                           !pNData->getShapeRect().IsEmpty());

    ScAddress aNDataStart = pNData->maStart;
    ScAddress aNDataEnd = pNData->maEnd;
    CPPUNIT_ASSERT_EQUAL(aDataStart, aNDataStart);
    CPPUNIT_ASSERT_EQUAL(aDataEnd, aNDataEnd);

    // Insert 2 rows.
    pDoc->InsertRow(ScRange(0, aDataStart.Row() - 1, 0, pDoc->MaxCol(), aDataStart.Row(), 0));

    // Get anchor data
    pData = ScDrawLayer::GetObjData(pObj);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve user data for this object.", pData);
    CPPUNIT_ASSERT_MESSAGE("Bounding rectangle should have been calculated upon import.",
                           !pData->getShapeRect().IsEmpty());

    // Get non rotated anchor data
    pNData = ScDrawLayer::GetNonRotatedObjData(pObj);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve non rotated user data for this object.", pNData);
    CPPUNIT_ASSERT_MESSAGE("Bounding rectangle should have been calculated upon import.",
                           !pNData->getShapeRect().IsEmpty());

    // Check if data has moved to new rows
    CPPUNIT_ASSERT_EQUAL(pData->maStart.Row(), aDataStart.Row() + 2);
    CPPUNIT_ASSERT_EQUAL(pData->maEnd.Row(), aDataEnd.Row() + 2);

    CPPUNIT_ASSERT_EQUAL(pNData->maStart.Row(), aNDataStart.Row() + 2);
    CPPUNIT_ASSERT_EQUAL(pNData->maEnd.Row(), aNDataEnd.Row() + 2);

    // Save the anchor data
    aDataStart = pData->maStart;
    aDataEnd = pData->maEnd;
    aNDataStart = pNData->maStart;
    aNDataEnd = pNData->maEnd;

    // Save the document and load again to check anchor persist
    saveAndReload("calc8");

    // There are two cell-anchored objects on the first sheet.
    pDoc = getScDoc();

    CPPUNIT_ASSERT_MESSAGE("There should be at least one sheet.", pDoc->GetTableCount() > 0);

    pDrawLayer = pDoc->GetDrawLayer();
    pPage = pDrawLayer->GetPage(0);
    CPPUNIT_ASSERT_MESSAGE("draw page for sheet 1 should exist.", pPage);
    pObj = pPage->GetObj(0);
    CPPUNIT_ASSERT_MESSAGE("Failed to get drawing object.", pObj);

    // Check cell anchor state
    oldType = ScDrawLayer::GetAnchorType(*pObj);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to get anchor type", SCA_CELL_RESIZE, oldType);

    // Get anchor data
    pData = ScDrawLayer::GetObjData(pObj);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve user data for this object.", pData);
    CPPUNIT_ASSERT_MESSAGE("Bounding rectangle should have been calculated upon import.",
                           !pData->getShapeRect().IsEmpty());

    // Get non rotated anchor data
    pNData = ScDrawLayer::GetNonRotatedObjData(pObj);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve non rotated user data for this object.", pNData);
    CPPUNIT_ASSERT_MESSAGE("Bounding rectangle should have been calculated upon import.",
                           !pNData->getShapeRect().IsEmpty());

    // Check if data after save it
    CPPUNIT_ASSERT_EQUAL(pData->maStart, aDataStart);
    CPPUNIT_ASSERT_EQUAL(pData->maEnd, aDataEnd);

    CPPUNIT_ASSERT_EQUAL(pNData->maStart, aNDataStart);
    CPPUNIT_ASSERT_EQUAL(pNData->maEnd, aNDataEnd);

    // Insert a column.
    pDoc->InsertCol(ScRange(aDataStart.Col(), 0, 0, aDataStart.Col(), pDoc->MaxRow(), 0));

    // Get anchor data
    pData = ScDrawLayer::GetObjData(pObj);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve user data for this object.", pData);
    CPPUNIT_ASSERT_MESSAGE("Bounding rectangle should have been calculated upon import.",
                           !pData->getShapeRect().IsEmpty());

    // Get non rotated anchor data
    pNData = ScDrawLayer::GetNonRotatedObjData(pObj);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve non rotated user data for this object.", pNData);
    CPPUNIT_ASSERT_MESSAGE("Bounding rectangle should have been calculated upon import.",
                           !pNData->getShapeRect().IsEmpty());

    // Check if data has moved to new rows
    CPPUNIT_ASSERT_EQUAL(pData->maStart.Col(), SCCOL(aDataStart.Col() + 1));
    CPPUNIT_ASSERT_EQUAL(pData->maEnd.Col(), SCCOL(aDataEnd.Col() + 1));

    CPPUNIT_ASSERT_EQUAL(pNData->maStart.Col(), SCCOL(aNDataStart.Col() + 1));
    CPPUNIT_ASSERT_EQUAL(pNData->maEnd.Col(), SCCOL(aNDataEnd.Col() + 1));

    // Save the anchor data
    aDataStart = pData->maStart;
    aDataEnd = pData->maEnd;
    aNDataStart = pNData->maStart;
    aNDataEnd = pNData->maEnd;

    // Save the document and load again to check anchor persist
    saveAndReload("calc8");

    // There are two cell-anchored objects on the first sheet.
    pDoc = getScDoc();

    CPPUNIT_ASSERT_MESSAGE("There should be at least one sheet.", pDoc->GetTableCount() > 0);

    pDrawLayer = pDoc->GetDrawLayer();
    pPage = pDrawLayer->GetPage(0);
    CPPUNIT_ASSERT_MESSAGE("draw page for sheet 1 should exist.", pPage);
    pObj = pPage->GetObj(0);
    CPPUNIT_ASSERT_MESSAGE("Failed to get drawing object.", pObj);

    // Check cell anchor state
    oldType = ScDrawLayer::GetAnchorType(*pObj);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to get anchor type", SCA_CELL_RESIZE, oldType);

    // Get anchor data
    pData = ScDrawLayer::GetObjData(pObj);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve user data for this object.", pData);
    CPPUNIT_ASSERT_MESSAGE("Bounding rectangle should have been calculated upon import.",
                           !pData->getShapeRect().IsEmpty());

    // Get non rotated anchor data
    pNData = ScDrawLayer::GetNonRotatedObjData(pObj);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve non rotated user data for this object.", pNData);
    CPPUNIT_ASSERT_MESSAGE("Bounding rectangle should have been calculated upon import.",
                           !pNData->getShapeRect().IsEmpty());

    // Check if data after save it
    CPPUNIT_ASSERT_EQUAL(pData->maStart, aDataStart);
    CPPUNIT_ASSERT_EQUAL(pData->maEnd, aDataEnd);

    CPPUNIT_ASSERT_EQUAL(pNData->maStart, aNDataStart);
    CPPUNIT_ASSERT_EQUAL(pNData->maEnd, aNDataEnd);
}

void ScExportTest::testConditionalFormatRangeListXLSX()
{
    createScDoc("ods/conditionalformat_rangelist.ods");
    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/worksheets/sheet1.xml");
    CPPUNIT_ASSERT(pDoc);
    assertXPath(pDoc, "//x:conditionalFormatting", "sqref", "F4 F10");
}

void ScExportTest::testConditionalFormatContainsTextXLSX()
{
    createScDoc("ods/conditionalformat_containstext.ods");
    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/worksheets/sheet1.xml");
    CPPUNIT_ASSERT(pDoc);
    assertXPathContent(pDoc, "//x:conditionalFormatting/x:cfRule/x:formula",
                       "NOT(ISERROR(SEARCH(\"test\",A1)))");
}

void ScExportTest::testConditionalFormatPriorityCheckXLSX()
{
    createScDoc("xlsx/conditional_fmt_checkpriority.xlsx");
    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/worksheets/sheet1.xml");
    CPPUNIT_ASSERT(pDoc);
    constexpr bool bHighPriorityExtensionA1
        = true; // Should A1's extension cfRule has higher priority than normal cfRule ?
    constexpr bool bHighPriorityExtensionA3
        = false; // Should A3's extension cfRule has higher priority than normal cfRule ?
    size_t nA1NormalPriority = 0;
    size_t nA1ExtPriority = 0;
    size_t nA3NormalPriority = 0;
    size_t nA3ExtPriority = 0;
    for (size_t nIdx = 1; nIdx <= 2; ++nIdx)
    {
        OString aIdx = OString::number(nIdx);
        OUString aCellAddr = getXPath(pDoc, "//x:conditionalFormatting[" + aIdx + "]", "sqref");
        OUString aPriority
            = getXPath(pDoc, "//x:conditionalFormatting[" + aIdx + "]/x:cfRule", "priority");
        CPPUNIT_ASSERT_MESSAGE("conditionalFormatting sqref must be either A1 or A3",
                               aCellAddr == "A1" || aCellAddr == "A3");
        if (aCellAddr == "A1")
            nA1NormalPriority = aPriority.toUInt32();
        else
            nA3NormalPriority = aPriority.toUInt32();
        aCellAddr = getXPathContent(
            pDoc, "//x:extLst/x:ext[1]/x14:conditionalFormattings/x14:conditionalFormatting[" + aIdx
                      + "]/xm:sqref");
        aPriority
            = getXPath(pDoc,
                       "//x:extLst/x:ext[1]/x14:conditionalFormattings/x14:conditionalFormatting["
                           + aIdx + "]/x14:cfRule",
                       "priority");
        CPPUNIT_ASSERT_MESSAGE("x14:conditionalFormatting sqref must be either A1 or A3",
                               aCellAddr == "A1" || aCellAddr == "A3");
        if (aCellAddr == "A1")
            nA1ExtPriority = aPriority.toUInt32();
        else
            nA3ExtPriority = aPriority.toUInt32();
    }
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong priorities for A1", bHighPriorityExtensionA1,
                                 nA1ExtPriority < nA1NormalPriority);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong priorities for A3", bHighPriorityExtensionA3,
                                 nA3ExtPriority < nA3NormalPriority);
}

void ScExportTest::testConditionalFormatOriginXLSX()
{
    createScDoc("xlsx/conditional_fmt_origin.xlsx");
    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/worksheets/sheet1.xml");
    CPPUNIT_ASSERT(pDoc);
    // tdf#124953 : The range-list is B3:C6 F1:G2, origin address in the formula should be B1, not B3.
    OUString aFormula = getXPathContent(pDoc, "//x:conditionalFormatting/x:cfRule/x:formula");
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong origin address in formula",
                                 OUString("NOT(ISERROR(SEARCH(\"BAC\",B1)))"), aFormula);
}

// FILESAVE: XLSX export with long sheet names (length > 31 characters)
void ScExportTest::testTdf79998()
{
    // check: original document has tab name > 31 characters
    createScDoc("ods/tdf79998.ods");
    ScDocument* pDoc = getScDoc();
    const std::vector<OUString> aTabNames1 = pDoc->GetAllTableNames();
    CPPUNIT_ASSERT_EQUAL(OUString("Utilities (FX Kurse, Kreditkarten etc)"), aTabNames1[1]);

    // check: saved XLSX document has truncated tab name
    saveAndReload("Calc Office Open XML");
    pDoc = getScDoc();
    const std::vector<OUString> aTabNames2 = pDoc->GetAllTableNames();
    CPPUNIT_ASSERT_EQUAL(OUString("Utilities (FX Kurse, Kreditkart"), aTabNames2[1]);
}

static void impl_testLegacyCellAnchoredRotatedShape(ScDocument& rDoc, const tools::Rectangle& aRect,
                                                    const ScDrawObjData& aAnchor,
                                                    tools::Long TOLERANCE = 30 /* 30 hmm */)
{
    ScDrawLayer* pDrawLayer = rDoc.GetDrawLayer();
    CPPUNIT_ASSERT_MESSAGE("No drawing layer.", pDrawLayer);
    SdrPage* pPage = pDrawLayer->GetPage(0);
    CPPUNIT_ASSERT_MESSAGE("No page instance for the 1st sheet.", pPage);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pPage->GetObjCount());

    SdrObject* pObj = pPage->GetObj(0);
    const tools::Rectangle& aSnap = pObj->GetSnapRect();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(aRect.GetHeight(), aSnap.GetHeight(), TOLERANCE);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(aRect.GetWidth(), aSnap.GetWidth(), TOLERANCE);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(aRect.Left(), aSnap.Left(), TOLERANCE);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(aRect.Top(), aSnap.Top(), TOLERANCE);

    ScDrawObjData* pData = ScDrawLayer::GetObjData(pObj);
    CPPUNIT_ASSERT_MESSAGE("expected object meta data", pData);
    CPPUNIT_ASSERT_EQUAL(aAnchor.maStart.Row(), pData->maStart.Row());
    CPPUNIT_ASSERT_EQUAL(aAnchor.maStart.Col(), pData->maStart.Col());
    CPPUNIT_ASSERT_EQUAL(aAnchor.maEnd.Row(), pData->maEnd.Row());
    CPPUNIT_ASSERT_EQUAL(aAnchor.maEnd.Col(), pData->maEnd.Col());
}

void ScExportTest::testLegacyCellAnchoredRotatedShape()
{
    {
        // This example doc contains cell anchored shape that is rotated, the
        // rotated shape is in fact clipped by the sheet boundaries (and thus
        // is a good edge case test to see if we import it still correctly)
        createScDoc("ods/legacycellanchoredrotatedclippedshape.ods");

        ScDocument* pDoc = getScDoc();
        // ensure the imported legacy rotated shape is in the expected position
        tools::Rectangle aRect(6000, -2000, 8000, 4000);
        // ensure the imported ( and converted ) anchor ( note we internally now store the anchor in
        // terms of the rotated shape ) is more or less contains the correct info
        ScDrawObjData aAnchor;
        aAnchor.maStart.SetRow(0);
        aAnchor.maStart.SetCol(5);
        aAnchor.maEnd.SetRow(3);
        aAnchor.maEnd.SetCol(7);
        impl_testLegacyCellAnchoredRotatedShape(*pDoc, aRect, aAnchor);
        // test save and reload
        // for some reason having this test in subsequent_export-test.cxx causes
        // a core dump in editeng ( so moved to here )
        saveAndReload("calc8");
        pDoc = getScDoc();
        impl_testLegacyCellAnchoredRotatedShape(*pDoc, aRect, aAnchor);
    }
    {
        // This example doc contains cell anchored shape that is rotated, the
        // rotated shape is in fact clipped by the sheet boundaries, additionally
        // the shape is completely hidden because the rows the shape occupies
        // are hidden
        createScDoc("ods/legacycellanchoredrotatedhiddenshape.ods");
        ScDocument* pDoc = getScDoc();
        // ensure the imported legacy rotated shape is in the expected position
        tools::Rectangle aRect(6000, -2000, 8000, 4000);

        // ensure the imported (and converted) anchor (note we internally now store the anchor in
        // terms of the rotated shape) is more or less contains the correct info
        ScDrawObjData aAnchor;
        aAnchor.maStart.SetRow(0);
        aAnchor.maStart.SetCol(5);
        aAnchor.maEnd.SetRow(3);
        aAnchor.maEnd.SetCol(7);
        pDoc->ShowRows(0, 9, 0, true); // show relevant rows
        pDoc->SetDrawPageSize(0); // trigger recalcpos
        impl_testLegacyCellAnchoredRotatedShape(*pDoc, aRect, aAnchor);
        // test save and reload
        saveAndReload("calc8");
        pDoc = getScDoc();
        impl_testLegacyCellAnchoredRotatedShape(*pDoc, aRect, aAnchor);
    }
    {
        // This example doc contains cell anchored shape that is rotated
        createScDoc("ods/legacycellanchoredrotatedshape.ods");

        ScDocument* pDoc = getScDoc();
        // ensure the imported legacy rotated shape is in the expected position
        tools::Rectangle aRect(6000, 3000, 8000, 9000);
        // ensure the imported (and converted) anchor (note we internally now store the anchor in
        // terms of the rotated shape) more or less contains the correct info

        ScDrawObjData aAnchor;
        aAnchor.maStart.SetRow(3);
        aAnchor.maStart.SetCol(6);
        aAnchor.maEnd.SetRow(9);
        aAnchor.maEnd.SetCol(8);
        // test import
        impl_testLegacyCellAnchoredRotatedShape(*pDoc, aRect, aAnchor);
        // test save and reload
        saveAndReload("calc8");
        pDoc = getScDoc();
        impl_testLegacyCellAnchoredRotatedShape(*pDoc, aRect, aAnchor);
    }
}

void ScExportTest::testTdf113646()
{
    createScDoc("ods/tdf113646.ods");

    save("Calc Office Open XML");
    xmlDocUniquePtr pSheet = parseExport("xl/styles.xml");
    CPPUNIT_ASSERT(pSheet);

    assertXPath(pSheet, "/x:styleSheet/x:dxfs/x:dxf/x:font/x:sz", "val", "36");
}

void ScExportTest::testDateStandardfilterXLSX()
{
    // XLSX Roundtripping standard filter with date
    createScDoc("ods/tdf142607.ods");

    save("Calc Office Open XML");
    xmlDocUniquePtr pDoc = parseExport("xl/worksheets/sheet1.xml");
    CPPUNIT_ASSERT(pDoc);

    assertXPath(pDoc, "//x:autoFilter", "ref", "A1:B6");
    assertXPath(pDoc, "//x:autoFilter/x:filterColumn/x:filters/x:dateGroupItem[1]", "day", "03");
    assertXPath(pDoc, "//x:autoFilter/x:filterColumn/x:filters/x:dateGroupItem[1]", "month", "12");
    assertXPath(pDoc, "//x:autoFilter/x:filterColumn/x:filters/x:dateGroupItem[1]", "year", "2011");
    assertXPath(pDoc, "//x:autoFilter/x:filterColumn/x:filters/x:dateGroupItem[1]",
                "dateTimeGrouping", "day");
}

void ScExportTest::testNumberFormatODS()
{
    createScDoc("ods/testNumberFormats.ods");
    saveAndReload("calc8");
    ScDocument* pDoc = getScDoc();
    sal_uInt32 nNumberFormat;
    const sal_Int32 nCountFormats = 18;
    const OUString aExpectedFormatStr[nCountFormats]
        = { "\"format=\"000000",        "\"format=\"??????",        "\"format=\"??0000",
            "\"format=\"000,000",       "\"format=\"???,???",       "\"format=\"??0,000",
            "\"format=\"000\" \"?/?",   "\"format=\"???\" \"?/?",   "\"format=\"?00\" \"?/?",
            "\"format=\"0,000\" \"?/?", "\"format=\"?,???\" \"?/?", "\"format=\"?,?00\" \"?/?",
            "\"format=\"0.000E+00",     "\"format=\"?.###E+00",     "\"format=\"?.0##E+00",
            "\"format=\"000E+00",       "\"format=\"???E+00",       "\"format=\"?00E+00" };
    for (sal_Int32 i = 0; i < nCountFormats; i++)
    {
        nNumberFormat = pDoc->GetNumberFormat(i + 1, 2, 0);
        const SvNumberformat* pNumberFormat = pDoc->GetFormatTable()->GetEntry(nNumberFormat);
        const OUString& rFormatStr = pNumberFormat->GetFormatstring();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Number format modified during export/import",
                                     aExpectedFormatStr[i], rFormatStr);
    }
    OUString aCSVPath = createFilePath(u"contentCSV/testNumberFormats.csv");
    testCondFile(aCSVPath, &*pDoc, 0,
                 false); // comma is thousand separator and cannot be used as delimiter
}

CPPUNIT_TEST_SUITE_REGISTRATION(ScExportTest);

CPPUNIT_PLUGIN_IMPLEMENT();

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
