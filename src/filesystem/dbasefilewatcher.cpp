/*
 * Copyright (C) 2017 ~ 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dbasefilewatcher.h"
#include "private/dbasefilewatcher_p.h"

#include <QEvent>
#include <QDebug>

DCORE_BEGIN_NAMESPACE

QList<DBaseFileWatcher*> DBaseFileWatcherPrivate::watcherList;
DBaseFileWatcherPrivate::DBaseFileWatcherPrivate(DBaseFileWatcher *qq)
    : DObjectPrivate(qq)
{

}

/*!
    \~english \class DBaseFileWatcher
    \~english \brief The DBaseFileWatcher class provides an interface for monitoring files and directories for modifications.
*/

/*!
    \~chinese \class DBaseFileWatcher
    \~chinese \brief DBaseFileWatcher 类提供了一系列接口可供监视文件和目录的变动。
*/

DBaseFileWatcher::~DBaseFileWatcher()
{
    stopWatcher();
    DBaseFileWatcherPrivate::watcherList.removeOne(this);
}

QUrl DBaseFileWatcher::fileUrl() const
{
    Q_D(const DBaseFileWatcher);

    return d->url;
}

/*!
 * \~chinese \brief 开始文件变动监视
 * \~english \brief Let file watcher start watching file changes.
 *
 * \sa stopWatcher(), restartWatcher()
 */
bool DBaseFileWatcher::startWatcher()
{
    Q_D(DBaseFileWatcher);

    if (d->started)
        return true;

    if (d->start()) {
        d->started = true;

        return true;
    }

    return false;
}

/*!
 * \~chinese \brief 停止文件变动监视
 * \~english \brief Stop watching file changes.
 *
 * \sa startWatcher(), restartWatcher()
 */
bool DBaseFileWatcher::stopWatcher()
{
    Q_D(DBaseFileWatcher);

    if (!d->started)
        return false;

    if (d->stop()) {
        d->started = false;

        return true;
    }

    return false;
}

/*!
 * \~chinese \brief 重新开始文件变动监视
 * \~english \brief Stop file watcher and then restart it to watching file changes.
 *
 * \sa startWatcher(), stopWatcher()
 */
bool DBaseFileWatcher::restartWatcher()
{
    bool ok = stopWatcher();
    return ok && startWatcher();
}

/*!
 * \~chinese \brief 设置是否对 \a subfileUrl 目录启用文件监视
 * \~english \brief Set enable file watcher for \a subfileUrl or not
 *
 * \~chinese \param subfileUrl 设置所针对的 Url
 * \~english \param subfileUrl The given url
 *
 * \~chinese \param enabled 是否启用文件变动监视
 * \~english \param enabled Enable file change watching or not.
 */
void DBaseFileWatcher::setEnabledSubfileWatcher(const QUrl &subfileUrl, bool enabled)
{
    Q_UNUSED(subfileUrl)
    Q_UNUSED(enabled)
}

/*!
 * \~chinese \brief 发送一个信号表示目标目录 \a targetUrl 得到了一个 \a signal 信号，包含参数 \a arg1 。
 * \~english \brief Emit a signal about \a targetUrl got a \a signal with \a arg1
 *
 * \~chinese 示例用法：
 * \~english Example usage:
 *
 * \~ \code{.cpp}
 * DBaseFileWatcher::ghostSignal(QUrl("bookmark:///"), &DBaseFileWatcher::fileDeleted, QUrl("bookmark:///bookmarkFile1"));
 * \endcode
 */
bool DBaseFileWatcher::ghostSignal(const QUrl &targetUrl, DBaseFileWatcher::SignalType1 signal, const QUrl &arg1)
{
    if (!signal)
        return false;

    bool ok = false;

    for (DBaseFileWatcher *watcher : DBaseFileWatcherPrivate::watcherList) {
        if (watcher->fileUrl() == targetUrl) {
            ok = true;
            (watcher->*signal)(arg1);
        }
    }

    return ok;
}

/*!
 * \~chinese \brief 发送一个信号表示目标目录 \a targetUrl 得到了一个 \a signal 信号，包含参数 \a arg1 和 \arg2。
 * \~english \brief Emit a signal about \a targetUrl got a \a signal with \a arg1 and \a arg2
 *
 * \~chinese 示例用法：
 * \~english Example usage:
 *
 * \~ \code{.cpp}
 * DBaseFileWatcher::ghostSignal(QUrl("bookmark:///"), &DBaseFileWatcher::fileMoved, QUrl("bookmark:///bookmarkFile1"), QUrl("bookmark:///NewNameFile1"));
 * \endcode
 */
bool DBaseFileWatcher::ghostSignal(const QUrl &targetUrl, DBaseFileWatcher::SignalType2 signal, const QUrl &arg1, const QUrl &arg2)
{
    if (!signal)
        return false;

    bool ok = false;

    for (DBaseFileWatcher *watcher : DBaseFileWatcherPrivate::watcherList) {
        if (watcher->fileUrl() == targetUrl) {
            ok = true;
            (watcher->*signal)(arg1, arg2);
        }
    }

    return ok;
}

DBaseFileWatcher::DBaseFileWatcher(DBaseFileWatcherPrivate &dd,
                                           const QUrl &url, QObject *parent)
    : QObject(parent)
    , DObject(dd)
{
    Q_ASSERT(url.isValid());

    d_func()->url = url;
    DBaseFileWatcherPrivate::watcherList << this;
}

DCORE_END_NAMESPACE

#include "moc_dbasefilewatcher.cpp"
