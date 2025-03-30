/*
 S PDX-FileCopyrightText*: 2025 Daniele Mte90 Scasciafratte <mte90net@gmail.com>

 SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KATEOLLAMACONFIGPAGE_H
#define KATEOLLAMACONFIGPAGE_H

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <KTextEditor/ConfigPage>

class KateOllamaPlugin;

class KateOllamaConfigPage : public KTextEditor::ConfigPage {
    Q_OBJECT

public:
    explicit KateOllamaConfigPage(QWidget *parent = nullptr, KateOllamaPlugin *plugin = nullptr);

    void saveSettings();
    void loadSettings();
    void fetchModelList();

public:
    QString name() const override;
    QString fullName() const override;
    QIcon icon() const override;

public:
    void apply() override;
    void defaults() override;
    void reset() override;

private:
    QComboBox *comboBox;
    QLineEdit *lineEdit;

    KateOllamaPlugin *m_plugin;
};

#endif // KATEOLLAMACONFIGPAGE_H
