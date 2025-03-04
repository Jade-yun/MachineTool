import xml.etree.ElementTree as ET
import csv
import os
from xml.dom import minidom

# 定义从 .ts 文件生成 .csv 文件的函数
def ts_to_csv(ts_file, csv_file):
    # 解析 .ts 文件
    tree = ET.parse(ts_file)
    root = tree.getroot()

    # 打开 CSV 文件准备写入
    with open(csv_file, 'w', newline='', encoding='utf-8') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(['SourceText', 'TranslatedText'])  # 写入表头

        # 遍历 .ts 文件中的所有 <message> 标签
        for context in root.findall('context'):
            for message in context.findall('message'):
                # 获取源文本
                source_text = message.find('source').text if message.find('source') is not None else ""
                
                # 获取翻译文本，如果没有翻译则为空字符串
                translation = message.find('translation')
                translated_text = translation.text if translation is not None else ""

                # 将源文本和翻译文本写入 CSV 文件
                writer.writerow([source_text, translated_text])

    print(f"CSV 文件已生成：{csv_file}")

def update_ts_file(ts_file, csv_file):
    # 读取 CSV 文件，生成源文本与翻译文本的映射
    translations = {}
    with open(csv_file, 'r', encoding='utf-8') as csvfile:
        reader = csv.reader(csvfile)
        next(reader)  # 跳过表头
        for row in reader:
            if len(row) >= 2:
                source_text = row[0]
                translated_text = row[1]
                translations[source_text] = translated_text

    # 解析现有的 .ts 文件
    tree = ET.parse(ts_file)
    root = tree.getroot()

    # 遍历所有的 <message> 标签并更新翻译
    for context in root.findall('context'):
        for message in context.findall('message'):
            source_text = message.find('source').text if message.find('source') is not None else ""
            if source_text in translations:
                # 更新翻译
                translated_text = translations[source_text]
                translation_node = message.find('translation')
                if translation_node is None:
                    translation_node = ET.SubElement(message, 'translation')
                translation_node.text = translated_text

    # 直接写入更新后的 .ts 文件（无额外空行）
    tree.write(ts_file, encoding='utf-8', xml_declaration=True)

    print(f"新的 .ts 文件已生成：{ts_file}")

from openpyxl import Workbook

def ts_to_excel(ts_file, excel_file):
    # 解析 .ts 文件
    tree = ET.parse(ts_file)
    root = tree.getroot()

    # 创建一个 Excel 工作簿
    wb = Workbook()
    ws = wb.active
    ws.title = "General"  # 初始创建一个通用的工作表，用来存放没有明确分类的文本

    # 遍历所有的 <context> 标签
    for context in root.findall('context'):
        context_name = context.find('name').text
        # 创建新的工作表，如果该工作表不存在的话
        if context_name not in wb.sheetnames:
            ws = wb.create_sheet(title=context_name)
            ws.append(['SourceText', 'TranslationText'])

        # 遍历每个 <message> 标签，获取 source 和 translation
        for message in context.findall('message'):
            source_text = message.find('source').text if message.find('source') is not None else ""
            translation = message.find('translation').text if message.find('translation') is not None else ""

            # 将源文本和翻译文本写入相应的 sheet
            ws.append([source_text, translation])

    # 保存 Excel 文件
    wb.save(excel_file)
    print(f"Excel 文件已生成：{excel_file}")

if __name__ == '__main__':
    ts_file = 'Translation_En.ts'
    csv_file = 'Translations.csv'
    excel_file = 'Translations_All.xlsx'  # 输出的 Excel 文件路径

    # 执行转换操作
    # ts_to_csv(ts_file, csv_file)
    # ts_to_excel(ts_file, excel_file)
    # 执行更新操作
    update_ts_file(ts_file, csv_file)
