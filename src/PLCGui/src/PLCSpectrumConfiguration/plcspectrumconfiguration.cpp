/*
 * plc-spectrum-manager.cc
 *
 *  Created on: 2012-06-05
 *      Author: deadlock
 */

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDialogButtonBox>
#include "plcspectrumconfiguration.h"
#include <QDebug>

PLCSpectrumConfiguration::PLCSpectrumConfiguration(PLCSpectrumModel* spectrumModel, QWidget *parent)
    : QDialog(parent)
{
    spectrum = spectrumModel;

	QVBoxLayout *layout = new QVBoxLayout();
	QHBoxLayout *innerLayout = new QHBoxLayout();

    QGroupBox *spectrumSettingsGroupBox = new QGroupBox();
    spectrumSettingsGroupBox->setTitle("Spectrum Model");

    QGridLayout *spectrumSettingsLayout = new QGridLayout();
    spectrumSettingsGroupBox->setLayout(spectrumSettingsLayout);

    spectrumSettingsLayout->addWidget(new QLabel("Lower Frequency Limit:"), 0, 0);
    spectrumSettingsLayout->addWidget(new QLabel("Band Resolution:"), 1, 0);
    spectrumSettingsLayout->addWidget(new QLabel("Frequency Bands:"), 2, 0);

    freqLimitLowBox = new QDoubleSpinBox();
    freqNumBandsBox = new QSpinBox();
    freqResBox = new QDoubleSpinBox();

    freqLimitLowBox->setRange(0, 99999);
    freqNumBandsBox->setRange(0, 99999);
    freqResBox->setRange(0, 99999);

    freqLimitLowBox->setDecimals(3);
    freqResBox->setDecimals(3);

    freqLimitLowBox->setSuffix(" kHz");
    freqResBox->setSuffix(" kHz");
    freqNumBandsBox->setSuffix("");

    freqLimitLowBox->setValue(spectrum->getLowerBandLimit()/1000.0);
    freqResBox->setValue(spectrum->getBandResolution()/1000.0);
    freqNumBandsBox->setValue((spectrum->getUpperBandLimit() - spectrum->getLowerBandLimit())/spectrum->getBandResolution());

    spectrumSettingsLayout->addWidget(freqLimitLowBox, 0, 1);
    spectrumSettingsLayout->addWidget(freqResBox, 1, 1);
    spectrumSettingsLayout->addWidget(freqNumBandsBox, 2, 1);

    connect(freqLimitLowBox, SIGNAL(valueChanged(double)), this, SLOT(saveValues()));
    connect(freqResBox, SIGNAL(valueChanged(double)), this, SLOT(saveValues()));
    connect(freqNumBandsBox, SIGNAL(valueChanged(int)), this, SLOT(saveValues()));

    QGroupBox *timeSettingsGroupBox = new QGroupBox();
    timeSettingsGroupBox->setTitle("Time model");
    QGridLayout *timeSettingsLayout = new QGridLayout();
    timeSettingsGroupBox->setLayout(timeSettingsLayout);

    timeSettingsLayout->addWidget(new QLabel("Main Frequency:"), 0, 0);
    timeSettingsLayout->addWidget(new QLabel("Samples per Cycle:"), 1, 0);
    timeSettingsLayout->addWidget(new QLabel("Symbol Length:"), 2, 0);

    mainsFreqBox = new QSpinBox();
    samplesPerCycleBox = new QSpinBox();
    simulationTimeBox = new QSpinBox();

    mainsFreqBox->setRange(1, 99999);
    samplesPerCycleBox->setRange(1, 99999);
    simulationTimeBox->setRange(0, 99999);

    mainsFreqBox->setSuffix(" Hz");
    samplesPerCycleBox->setSuffix("");
    simulationTimeBox->setSuffix(" us");

    mainsFreqBox->setValue(spectrum->getMainsFrequency());
    samplesPerCycleBox->setValue(spectrum->getSamplesPerCycle());
    simulationTimeBox->setValue(spectrum->getSymbolLength());

    timeSettingsLayout->addWidget(mainsFreqBox, 0, 1);
    timeSettingsLayout->addWidget(samplesPerCycleBox, 1, 1);
    timeSettingsLayout->addWidget(simulationTimeBox, 2, 1);

    connect(mainsFreqBox, SIGNAL(valueChanged(int)), this, SLOT(saveValues()));
    connect(samplesPerCycleBox, SIGNAL(valueChanged(int)), this, SLOT(saveValues()));
    connect(simulationTimeBox, SIGNAL(valueChanged(int)), this, SLOT(saveValues()));

    innerLayout->addWidget(spectrumSettingsGroupBox);
    innerLayout->addWidget(timeSettingsGroupBox);
	layout->addLayout(innerLayout);

	QDialogButtonBox *dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Horizontal);
	okButton = dialogButtonBox->addButton(QDialogButtonBox::Ok);
	connect(dialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(dialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

	layout->addWidget(dialogButtonBox, 0, Qt::AlignCenter);

	saveValues();

	this->setLayout(layout);
}


void PLCSpectrumConfiguration::saveValues(void)
{
    spectrum->setLowerBandLimit(freqLimitLowBox->value()*1000.0);
    spectrum->setBandResolution(freqResBox->value()*1000.0);
    spectrum->setUpperBandLimit(spectrum->getLowerBandLimit() + (spectrum->getBandResolution() * freqNumBandsBox->value()));

    spectrum->setMainsFrequency(mainsFreqBox->value());
    spectrum->setSymbolLength(simulationTimeBox->value());

    spectrum->setSamplesPerCycle(samplesPerCycleBox->value());

    if ((spectrum->getLowerBandLimit() < spectrum->getUpperBandLimit() && spectrum->getBandResolution() > 0) &&
            ((spectrum->getUpperBandLimit() - spectrum->getLowerBandLimit()) / 2.0) > spectrum->getBandResolution())
	{
		okButton->setDisabled(false);
	}
	else
	{
		okButton->setDisabled(true);
	}
}
