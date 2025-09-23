#include "SyntaxHighlighter.h"
#include <QtGui/QColor>

JsonSyntaxHighlighter::JsonSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    // JSON key format (quoted strings followed by colon)
    keyFormat.setForeground(QColor(86, 156, 214)); // Light blue
    keyFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression(QStringLiteral("\"[^\"]*\"(?=\\s*:)"));
    rule.format = keyFormat;
    highlightingRules.append(rule);

    // JSON string values
    stringFormat.setForeground(QColor(206, 145, 120)); // Light orange
    rule.pattern = QRegularExpression(QStringLiteral("(?<=:)\\s*\"[^\"]*\""));
    rule.format = stringFormat;
    highlightingRules.append(rule);

    // JSON numbers
    numberFormat.setForeground(QColor(181, 206, 168)); // Light green
    rule.pattern = QRegularExpression(QStringLiteral("\\b[+-]?\\d+(?:\\.\\d+)?(?:[eE][+-]?\\d+)?\\b"));
    rule.format = numberFormat;
    highlightingRules.append(rule);

    // JSON boolean values
    booleanFormat.setForeground(QColor(86, 156, 214)); // Light blue
    booleanFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression(QStringLiteral("\\b(true|false)\\b"));
    rule.format = booleanFormat;
    highlightingRules.append(rule);

    // JSON null
    nullFormat.setForeground(QColor(156, 156, 156)); // Gray
    nullFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression(QStringLiteral("\\bnull\\b"));
    rule.format = nullFormat;
    highlightingRules.append(rule);

    // JSON punctuation
    punctuationFormat.setForeground(QColor(220, 220, 170)); // Light yellow
    rule.pattern = QRegularExpression(QStringLiteral("[{}\\[\\],:]"));
    rule.format = punctuationFormat;
    highlightingRules.append(rule);
}

void JsonSyntaxHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}

XmlSyntaxHighlighter::XmlSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    // XML element format
    xmlElementFormat.setForeground(QColor(86, 156, 214)); // Light blue
    xmlElementFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression(QStringLiteral("</?\\b[A-Za-z_][A-Za-z0-9_-]*\\b"));
    rule.format = xmlElementFormat;
    highlightingRules.append(rule);

    // XML attribute names
    xmlKeywordFormat.setForeground(QColor(156, 220, 254)); // Lighter blue
    rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z_][A-Za-z0-9_-]*(?=\\s*=)"));
    rule.format = xmlKeywordFormat;
    highlightingRules.append(rule);

    // XML attribute values
    xmlValueFormat.setForeground(QColor(206, 145, 120)); // Light orange
    rule.pattern = QRegularExpression(QStringLiteral("\"[^\"]*\""));
    rule.format = xmlValueFormat;
    highlightingRules.append(rule);

    // XML comments
    xmlCommentFormat.setForeground(QColor(106, 153, 85)); // Green
    xmlCommentFormat.setFontItalic(true);
    rule.pattern = QRegularExpression(QStringLiteral("<!--[^>]*-->"));
    rule.format = xmlCommentFormat;
    highlightingRules.append(rule);

    // XML tags (< and >)
    QTextCharFormat xmlTagFormat;
    xmlTagFormat.setForeground(QColor(220, 220, 170)); // Light yellow
    rule.pattern = QRegularExpression(QStringLiteral("[<>/?=]"));
    rule.format = xmlTagFormat;
    highlightingRules.append(rule);
}

void XmlSyntaxHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}