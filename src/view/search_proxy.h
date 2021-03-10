/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DEEPIN_MANUAL_VIEW_SEARCH_PROXY_H
#define DEEPIN_MANUAL_VIEW_SEARCH_PROXY_H

#include <QObject>

class SearchProxy : public QObject
{
    Q_OBJECT
public:
    explicit SearchProxy(QObject *parent = nullptr);
    ~SearchProxy() override;

signals:
    void mismatch(const QString &keyword);
    void onContentResult(const QString &app_name,
                         const QStringList &anchors,
                         const QStringList &anchorIdList,
                         const QStringList &contents);
    void reloadPage(const QStringList &appList);
    void setKeyword(const QString &keyword);
    void updateSearchResult();

public slots:
    void getKeyword(const QString &keyword);
    void updateSearch(const QString &keyword);
};

#endif // DEEPIN_MANUAL_VIEW_SEARCH_PROXY_H
