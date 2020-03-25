/****************************************************************************
**
** Copyright (C) 2020 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtQml module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**/
#ifndef ERRORMESSAGE_H
#define ERRORMESSAGE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qqmldom_global.h"
#include "qqmldomstringdumper_p.h"
#include "qqmldompath_p.h"

#include <QtQml/private/qqmljsast_p.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtCore/QCborArray>
#include <QtCore/QCborMap>
#include <QtQml/private/qqmljsdiagnosticmessage_p.h>

QT_BEGIN_NAMESPACE

namespace QQmlJS {
namespace Dom {

QMLDOM_EXPORT ErrorLevel errorLevelFromQtMsgType(QtMsgType msgType);

class ErrorGroups;
class DomItem;
using std::function;

#define NewErrorGroup(name) QQmlJS::Dom::ErrorGroup(QT_TRANSLATE_NOOP("ErrorGroup", name))

class QMLDOM_EXPORT ErrorGroup {
    Q_GADGET
    Q_DECLARE_TR_FUNCTIONS(ErrorGroup)
public:
    constexpr ErrorGroup(const char *groupId):
        m_groupId(groupId)
    {}


    void dump(Sink sink) const;
    void dumpId(Sink sink) const;

    QLatin1String groupId() const;
    QString groupName() const;
 private:
    const char *m_groupId;
};

class QMLDOM_EXPORT ErrorGroups{
    Q_GADGET
public:
    void dump(Sink sink) const;
    void dumpId(Sink sink) const;
    QCborArray toCbor() const;

    [[nodiscard]] ErrorMessage errorMessage(Dumper msg, ErrorLevel level, Path element = Path(), QString canonicalFilePath = QString(), SourceLocation location = SourceLocation()) const;
    [[nodiscard]] ErrorMessage errorMessage(const DiagnosticMessage &msg, Path element = Path(), QString canonicalFilePath = QString()) const;

    void fatal(Dumper msg, Path element = Path(), QStringView canonicalFilePath = u"", SourceLocation location = SourceLocation()) const;

    [[nodiscard]] ErrorMessage debug(QString message) const;
    [[nodiscard]] ErrorMessage debug(Dumper message) const;
    [[nodiscard]] ErrorMessage info(QString message) const;
    [[nodiscard]] ErrorMessage info(Dumper message) const;
    [[nodiscard]] ErrorMessage warning(QString message) const;
    [[nodiscard]] ErrorMessage warning(Dumper message) const;
    [[nodiscard]] ErrorMessage error(QString message) const;
    [[nodiscard]] ErrorMessage error(Dumper message) const;

    static int cmp(const ErrorGroups &g1, const ErrorGroups &g2);

    QVector<ErrorGroup> groups;
};

inline bool operator==(const ErrorGroups& lhs, const ErrorGroups& rhs){ return ErrorGroups::cmp(lhs,rhs) == 0; }
inline bool operator!=(const ErrorGroups& lhs, const ErrorGroups& rhs){ return ErrorGroups::cmp(lhs,rhs) != 0; }
inline bool operator< (const ErrorGroups& lhs, const ErrorGroups& rhs){ return ErrorGroups::cmp(lhs,rhs) <  0; }
inline bool operator> (const ErrorGroups& lhs, const ErrorGroups& rhs){ return ErrorGroups::cmp(lhs,rhs) >  0; }
inline bool operator<=(const ErrorGroups& lhs, const ErrorGroups& rhs){ return ErrorGroups::cmp(lhs,rhs) <= 0; }
inline bool operator>=(const ErrorGroups& lhs, const ErrorGroups& rhs){ return ErrorGroups::cmp(lhs,rhs) >= 0; }

class QMLDOM_EXPORT ErrorMessage { // reuse Some of the other DiagnosticMessages?
    Q_GADGET
    Q_DECLARE_TR_FUNCTIONS(ErrorMessage)
public:
    using Level = ErrorLevel;
    // error registry (usage is optional)
    static QLatin1String msg(const char *errorId, ErrorMessage err);
    static QLatin1String msg(QLatin1String errorId, ErrorMessage err);
    static void visitRegisteredMessages(function_ref<bool(ErrorMessage)> visitor);
    [[nodiscard]] static ErrorMessage load(QLatin1String errorId);
    [[nodiscard]] static ErrorMessage load(const char *errorId);
    template<typename... T>
    [[nodiscard]] static ErrorMessage load(QLatin1String errorId, T... args){
        ErrorMessage res = load(errorId);
        res.message = res.message.arg(args...);
        return res;
    }

    ErrorMessage(QString message, ErrorGroups errorGroups, Level level = Level::Warning, Path path = Path(), QString file = QString(), SourceLocation location = SourceLocation(), QLatin1String errorId = QLatin1String(""));
    ErrorMessage(ErrorGroups errorGroups, const DiagnosticMessage &msg, Path path = Path(), QString file = QString(), QLatin1String errorId = QLatin1String(""));

    [[nodiscard]] ErrorMessage &withErrorId(QLatin1String errorId);
    [[nodiscard]] ErrorMessage &withPath(const Path &);
    [[nodiscard]] ErrorMessage &withFile(QString);
    [[nodiscard]] ErrorMessage &withFile(QStringView);
    [[nodiscard]] ErrorMessage &withLocation(SourceLocation);
    [[nodiscard]] ErrorMessage &withItem(DomItem);

    ErrorMessage handle(const ErrorHandler &errorHandler=nullptr);

    void dump(Sink s) const;
    QString toString() const;
    QCborMap toCbor() const;

    QLatin1String errorId;
    QString message;
    ErrorGroups errorGroups;
    Level level;
    Path path;
    QString file;
    SourceLocation location;
};

inline bool operator !=(const ErrorMessage &e1, const ErrorMessage &e2) {
    return e1.errorId != e2.errorId || e1.message != e2.message || e1.errorGroups != e2.errorGroups
            || e1.level != e2.level || e1.path != e2.path || e1.file != e2.file
            || e1.location.startLine != e2.location.startLine || e1.location.offset != e2.location.offset
            || e1.location.startColumn != e2.location.startColumn || e1.location.length != e2.location.length;
}
inline bool operator ==(const ErrorMessage &e1, const ErrorMessage &e2) {
    return !(e1 != e2);
}

QMLDOM_EXPORT void silentError(const ErrorMessage &);
QMLDOM_EXPORT void errorToQDebug(const ErrorMessage &);

QMLDOM_EXPORT void defaultErrorHandler(const ErrorMessage &);
QMLDOM_EXPORT void setDefaultErrorHandler(ErrorHandler h);

} // end namespace Dom
} // end namespace QQmlJS
QT_END_NAMESPACE
#endif // ERRORMESSAGE_H