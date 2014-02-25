/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 * Version 0.6b
 *
 * Copyright (c) 2012-2014 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Main file authors: Alexander Johansson, Timo Ropinski
 *
 *********************************************************************************/

#include <inviwo/qt/widgets/properties/floatpropertywidgetqt.h>

namespace inviwo {

BaseOrdinalPropertyWidgetQt::BaseOrdinalPropertyWidgetQt(Property* property)
    : PropertyWidgetQt(property)
    , contextMenu_(NULL)
    , settingsWidget_(NULL) {
}

BaseOrdinalPropertyWidgetQt::~BaseOrdinalPropertyWidgetQt() {
}

void BaseOrdinalPropertyWidgetQt::generateWidget() {
    signalMapperSetPropertyValue_ = new QSignalMapper(this);
    signalMapperContextMenu_ = new QSignalMapper(this);
    
    QHBoxLayout* hLayout = new QHBoxLayout();
    
    hLayout->setContentsMargins(0,0,0,0);
    hLayout->setSpacing(0);

    label_ = new EditableLabelQt(this,
                                 property_->getDisplayName(),
                                 PropertyWidgetQt::getContextMenu());
    hLayout->addWidget(label_);

    QSizePolicy labelPol = label_->sizePolicy();
    labelPol.setHorizontalStretch(1);
    label_->setSizePolicy(labelPol);
    connect(label_, SIGNAL(textChanged()),this, SLOT(setPropertyDisplayName()));

       
    QWidget* sliderWidget = new QWidget();
    QSizePolicy sliderPol = sliderWidget->sizePolicy();
    sliderPol.setHorizontalStretch(3);
    sliderWidget->setSizePolicy(sliderPol);

    QVBoxLayout* vLayout = new QVBoxLayout();
    sliderWidget->setLayout(vLayout);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);

    sliderWidgets_ = makeSliders();

    for(size_t i = 0; i < sliderWidgets_.size(); i++) {
        sliderWidgets_[i]->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(sliderWidgets_[i],
                SIGNAL(customContextMenuRequested(const QPoint&)),
                signalMapperContextMenu_,
                SLOT(map()));

        connect(sliderWidgets_[i],
                SIGNAL(valueChanged()),
                signalMapperSetPropertyValue_,
                SLOT(map()));

        signalMapperContextMenu_->setMapping(sliderWidgets_[i], i);
        signalMapperSetPropertyValue_->setMapping(sliderWidgets_[i], i);

        vLayout->addWidget(sliderWidgets_[i]);
    }

    hLayout->addWidget(sliderWidget);

    connect(signalMapperContextMenu_,
            SIGNAL(mapped(int)),
            this,
            SLOT(showContextMenuSlider(int)));

    connect(signalMapperSetPropertyValue_,
            SIGNAL(mapped(int)),
            this,
            SLOT(setPropertyValue(int)));

    this->setEnabled(!property_->getReadOnly());
    setLayout(hLayout);
}


void BaseOrdinalPropertyWidgetQt::generatesSettingsWidget() {

    settingsAction_ = new QAction(tr("&Property settings..."), this);
    minAction_ = new QAction(tr("&Set as Min"), this);
    maxAction_ = new QAction(tr("&Set as Max"), this);

    connect(settingsAction_,
            SIGNAL(triggered()),
            this,
            SLOT(showSettings()));

    connect(minAction_,
            SIGNAL(triggered()),
            this,
            SLOT(setAsMin()));
    
    connect(maxAction_,
            SIGNAL(triggered()),
            this,
            SLOT(setAsMax()));
    
    contextMenu_ = new QMenu();
    contextMenu_->addActions(PropertyWidgetQt::getContextMenu()->actions());
    contextMenu_->addAction(settingsAction_);
    contextMenu_->addAction(minAction_);
    contextMenu_->addAction(maxAction_);
}

/****************************************************************************/
// Slots:

// Connected to label_ textChanged
void BaseOrdinalPropertyWidgetQt::setPropertyDisplayName() {
    property_->setDisplayName(label_->getText());
}

// connected to sliderWidget_ customContextMenuRequested
void BaseOrdinalPropertyWidgetQt::showContextMenuSlider(int sliderId) {
    sliderId_ = sliderId;

    if (!contextMenu_){
        generatesSettingsWidget();
    }

    PropertyWidgetQt::updateContextMenu();
    PropertyVisibilityMode appVisibilityMode  = getApplicationViewMode();

    if (appVisibilityMode == DEVELOPMENT) {
        settingsAction_->setVisible(true);
        minAction_->setVisible(true);
        maxAction_->setVisible(true);
    }else{
        settingsAction_->setVisible(false);
        minAction_->setVisible(false);
        maxAction_->setVisible(false);
    }
    
    if (property_->getReadOnly()){
        settingsAction_->setEnabled(false);
        minAction_->setEnabled(false);
        maxAction_->setEnabled(false);
    }else{
        settingsAction_->setEnabled(true);
        minAction_->setEnabled(true);
        maxAction_->setEnabled(true);
    }
    contextMenu_->exec(QCursor::pos());

}

void BaseOrdinalPropertyWidgetQt::showSettings(){
    if (!settingsWidget_){
        settingsWidget_ = new PropertySettingsWidgetQt(property_, this);
    }
    settingsWidget_->reload();
    settingsWidget_->show();
}



} // namespace
