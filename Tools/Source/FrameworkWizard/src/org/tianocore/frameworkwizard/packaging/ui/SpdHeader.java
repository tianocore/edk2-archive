/** @file
 
 The file is used to create, update spdHeader of Spd file
 
 Copyright (c) 2006, Intel Corporation
 All rights reserved. This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution.  The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php
 
 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 
 **/

package org.tianocore.frameworkwizard.packaging.ui;

import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ComponentEvent;

import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;

import org.tianocore.FrameworkComponentTypes;
import org.tianocore.LicenseDocument;
import org.tianocore.PackageType;
import org.tianocore.SpdHeaderDocument;
import org.tianocore.SpecificationDocument;
import org.tianocore.URLDocument;
import org.tianocore.frameworkwizard.common.DataType;
import org.tianocore.frameworkwizard.common.DataValidation;
import org.tianocore.frameworkwizard.common.Log;
import org.tianocore.frameworkwizard.common.Tools;
import org.tianocore.frameworkwizard.common.ui.IInternalFrame;
import org.tianocore.frameworkwizard.common.ui.StarLabel;

/**
 The class is used to create, update spdHeader of Spd file
 It extends IInternalFrame
 
 @since PackageEditor 1.0

 **/
public class SpdHeader extends IInternalFrame {

    ///
    /// Define class Serial Version UID
    ///
    private static final long serialVersionUID = -8152099582923006900L;

    //
    //Define class members
    //
    private JPanel jContentPane = null;

    private JLabel jLabelBaseName = null;

    private JTextField jTextFieldBaseName = null;

    private JLabel jLabelGuid = null;

    private JTextField jTextFieldGuid = null;

    private JLabel jLabelVersion = null;

    private JTextField jTextFieldVersion = null;

    private JButton jButtonGenerateGuid = null;

    private JLabel jLabelLicense = null;

    private JTextArea jTextAreaLicense = null;

    private JLabel jLabelCopyright = null;

    private JLabel jLabelDescription = null;

    private JTextArea jTextAreaDescription = null;

    private JLabel jLabelSpecification = null;

    private JTextField jTextFieldSpecification = null;

    private JButton jButtonOk = null;

    private JButton jButtonCancel = null;

    private JScrollPane jScrollPaneLicense = null;

    private JScrollPane jScrollPaneDescription = null;

    private JLabel jLabelAbstract = null;

    private JTextField jTextFieldAbstract = null;

    private JLabel jLabelPackageType = null;

    private JLabel jLabelReadOnly = null;
    
    private JLabel jLabelRePackage = null;

    private JComboBox jComboBoxReadOnly = null;
    
    private JComboBox jComboBoxRePackage = null;

    private JComboBox jComboBoxPackageType = null;

    private StarLabel jStarLabel1 = null;

    private StarLabel jStarLabel2 = null;

    private StarLabel jStarLabel3 = null;

    private StarLabel jStarLabel4 = null;

    private StarLabel jStarLabel5 = null;

    private StarLabel jStarLabel6 = null;

    private StarLabel jStarLabel7 = null;

    private StarLabel jStarLabel8 = null;

    private StarLabel jStarLabel9 = null;
    
    private StarLabel jStarLabel10 = null;

    private SpdHeaderDocument.SpdHeader spdHeader = null;

    private JTextField jTextFieldCopyright = null;

    /**
     This method initializes jTextFieldBaseName 
     
     @return javax.swing.JTextField jTextFieldBaseName
     
     **/
    private JTextField getJTextFieldBaseName() {
        if (jTextFieldBaseName == null) {
            jTextFieldBaseName = new JTextField();
            jTextFieldBaseName.setBounds(new java.awt.Rectangle(160, 10, 320, 20));
            jTextFieldBaseName.setPreferredSize(new java.awt.Dimension(320,20));
        }
        return jTextFieldBaseName;
    }

    /**
     This method initializes jTextFieldGuid 
     
     @return javax.swing.JTextField jTextFieldGuid
     
     **/
    private JTextField getJTextFieldGuid() {
        if (jTextFieldGuid == null) {
            jTextFieldGuid = new JTextField();
            jTextFieldGuid.setBounds(new java.awt.Rectangle(160, 35, 250, 20));
            jTextFieldGuid.setPreferredSize(new java.awt.Dimension(250,20));
        }
        return jTextFieldGuid;
    }

    /**
     This method initializes jTextFieldVersion 
     
     @return javax.swing.JTextField jTextFieldVersion
     
     **/
    private JTextField getJTextFieldVersion() {
        if (jTextFieldVersion == null) {
            jTextFieldVersion = new JTextField();
            jTextFieldVersion.setBounds(new java.awt.Rectangle(160, 60, 320, 20));
            jTextFieldVersion.setPreferredSize(new java.awt.Dimension(320,20));
        }
        return jTextFieldVersion;
    }

    /**
     This method initializes jButtonGenerateGuid 
     
     @return javax.swing.JButton jButtonGenerateGuid
     
     **/
    private JButton getJButtonGenerateGuid() {
        if (jButtonGenerateGuid == null) {
            jButtonGenerateGuid = new JButton();
            jButtonGenerateGuid.setBounds(new java.awt.Rectangle(415, 35, 65, 20));
            jButtonGenerateGuid.setText("GEN");
            jButtonGenerateGuid.addActionListener(this);
        }
        return jButtonGenerateGuid;
    }

    /**
     This method initializes jTextAreaLicense 
     
     @return javax.swing.JTextArea jTextAreaLicense
     
     **/
    private JTextArea getJTextAreaLicense() {
        if (jTextAreaLicense == null) {
            jTextAreaLicense = new JTextArea();
            jTextAreaLicense.setText("");
            jTextAreaLicense.setPreferredSize(new java.awt.Dimension(317,77));
            jTextAreaLicense.setLineWrap(true);
        }
        return jTextAreaLicense;
    }

    /**
     This method initializes jTextAreaDescription 
     
     @return javax.swing.JTextArea jTextAreaDescription
     
     **/
    private JTextArea getJTextAreaDescription() {
        if (jTextAreaDescription == null) {
            jTextAreaDescription = new JTextArea();
            jTextAreaDescription.setLineWrap(true);
            jTextAreaDescription.setPreferredSize(new java.awt.Dimension(317,77));
        }
        return jTextAreaDescription;
    }

    /**
     This method initializes jTextFieldSpecification 
     
     @return javax.swing.JTextField jTextFieldSpecification
     
     **/
    private JTextField getJTextFieldSpecification() {
        if (jTextFieldSpecification == null) {
            jTextFieldSpecification = new JTextField();
            jTextFieldSpecification.setBounds(new java.awt.Rectangle(160, 280, 320, 20));
            jTextFieldSpecification.setPreferredSize(new java.awt.Dimension(320,20));
        }
        return jTextFieldSpecification;
    }

    /**
     This method initializes jButtonOk 
     
     @return javax.swing.JButton jButtonOk
     
     **/
    private JButton getJButtonOk() {
        if (jButtonOk == null) {
            jButtonOk = new JButton();
            jButtonOk.setText("OK");
            jButtonOk.setBounds(new java.awt.Rectangle(290, 445, 90, 20));
            jButtonOk.addActionListener(this);
        }
        return jButtonOk;
    }

    /**
     This method initializes jButtonCancel 
     
     @return javax.swing.JButton jButtonCancel
     
     **/
    private JButton getJButtonCancel() {
        if (jButtonCancel == null) {
            jButtonCancel = new JButton();
            jButtonCancel.setText("Cancel");
            jButtonCancel.setBounds(new java.awt.Rectangle(390, 445, 90, 20));
            jButtonCancel.addActionListener(this);
        }
        return jButtonCancel;
    }

    /**
     This method initializes jScrollPaneLicense 
     
     @return javax.swing.JScrollPane jScrollPaneLicense
     
     **/
    private JScrollPane getJScrollPaneLicense() {
        if (jScrollPaneLicense == null) {
            jScrollPaneLicense = new JScrollPane();
            jScrollPaneLicense.setBounds(new java.awt.Rectangle(160, 85, 320, 80));
            jScrollPaneLicense.setHorizontalScrollBarPolicy(javax.swing.JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
            jScrollPaneLicense.setPreferredSize(new java.awt.Dimension(320,80));
            jScrollPaneLicense.setViewportView(getJTextAreaLicense());
        }
        return jScrollPaneLicense;
    }

    /**
     This method initializes jScrollPaneDescription 
     
     @return javax.swing.JScrollPane jScrollPaneDescription
     
     **/
    private JScrollPane getJScrollPaneDescription() {
        if (jScrollPaneDescription == null) {
            jScrollPaneDescription = new JScrollPane();
            jScrollPaneDescription.setBounds(new java.awt.Rectangle(160, 195, 320, 80));
            jScrollPaneDescription.setHorizontalScrollBarPolicy(javax.swing.JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
            jScrollPaneDescription.setViewportView(getJTextAreaDescription());
        }
        return jScrollPaneDescription;
    }

    /**
     This method initializes jTextFieldAbstract 
     
     @return javax.swing.JTextField jTextFieldAbstract
     
     **/
    private JTextField getJTextFieldAbstract() {
        if (jTextFieldAbstract == null) {
            jTextFieldAbstract = new JTextField();
            jTextFieldAbstract.setBounds(new java.awt.Rectangle(160,305,320,20));
            jTextFieldAbstract.setPreferredSize(new java.awt.Dimension(320, 20));
        }
        return jTextFieldAbstract;
    }

    /**
     This method initializes jComboBoxReadOnly 
     
     @return javax.swing.JComboBox jComboBoxReadOnly
     
     **/
    private JComboBox getJComboBoxReadOnly() {
        if (jComboBoxReadOnly == null) {
            jComboBoxReadOnly = new JComboBox();
            jComboBoxReadOnly.setBounds(new java.awt.Rectangle(160,380,80,20));
            jComboBoxReadOnly.setPreferredSize(new java.awt.Dimension(60,20));
        }
        return jComboBoxReadOnly;
    }
    
    private JComboBox getJComboBoxRePackage() {
        if (jComboBoxRePackage == null) {
            jComboBoxRePackage = new JComboBox();
            jComboBoxRePackage.setBounds(new java.awt.Rectangle(160,405,80,20));
            jComboBoxRePackage.setPreferredSize(new java.awt.Dimension(60,20));
        }
        return jComboBoxRePackage;
    }

    /**
     This method initializes jComboBoxPackageType 
     
     @return javax.swing.JComboBox jComboBoxPackageType
     
     **/
    private JComboBox getJComboBoxPackageType() {
        if (jComboBoxPackageType == null) {
            jComboBoxPackageType = new JComboBox();
            jComboBoxPackageType.setBounds(new java.awt.Rectangle(160, 355, 320, 20));
            jComboBoxPackageType.setPreferredSize(new java.awt.Dimension(320,20));
        }
        return jComboBoxPackageType;
    }

    /**
      This method initializes jTextFieldCopyright	
      	
      @return javax.swing.JTextField jTextFieldCopyright
     
     **/
    private JTextField getJTextFieldCopyright() {
        if (jTextFieldCopyright == null) {
            jTextFieldCopyright = new JTextField();
            jTextFieldCopyright.setBounds(new java.awt.Rectangle(160,170,320, 20));
            jTextFieldCopyright.setPreferredSize(new java.awt.Dimension(320,20));
        }
        return jTextFieldCopyright;
    }

    public static void main(String[] args) {
        new SpdHeader().setVisible(true);
    }

    /**
     This is the default constructor
     
     **/
    public SpdHeader() {
        super();
        init();
        this.setVisible(true);
    }

    /**
     This is the override edit constructor
     
     @param inspdHeader The input data of spdHeaderDocument.spdHeader
     
     **/
    public SpdHeader(SpdHeaderDocument.SpdHeader inspdHeader) {
        super();
        init(inspdHeader);
        this.setVisible(true);
        this.setViewMode(false);
    }

    /**
     Disable all components when the mode is view
     
     @param isView true - The view mode; false - The non-view mode
     
     **/
    public void setViewMode(boolean isView) {
        this.jButtonOk.setVisible(false);
        this.jButtonCancel.setVisible(false);
        if (isView) {
            this.jTextFieldBaseName.setEnabled(!isView);
            this.jTextFieldGuid.setEnabled(!isView);
            this.jTextFieldVersion.setEnabled(!isView);
            this.jTextAreaLicense.setEnabled(!isView);
            this.jTextFieldCopyright.setEnabled(!isView);
            this.jTextAreaDescription.setEnabled(!isView);
            this.jTextFieldSpecification.setEnabled(!isView);            
            this.jTextFieldAbstract.setEnabled(!isView);
            this.jComboBoxPackageType.setEnabled(!isView);
            this.jComboBoxReadOnly.setEnabled(!isView);
            this.jButtonCancel.setEnabled(!isView);
            this.jButtonGenerateGuid.setEnabled(!isView);
            this.jButtonOk.setEnabled(!isView);
        }
    }

    /**
     This method initializes this
     
     **/
    private void init() {
        //this.setSize(500, 515);
        this.setContentPane(getJContentPane());
        this.setTitle("Package Surface Area Header");
        initFrame();
    }

    /**
     This method initializes this
     Fill values to all fields if these values are not empty
     
     @param inspdHeader  The input data of spdHeaderDocument.spdHeader
     
     **/
    private void init(SpdHeaderDocument.SpdHeader inspdHeader) {
        init();
        if (inspdHeader != null) {
            setSpdHeader(inspdHeader);
            if (this.spdHeader.getPackageName() != null) {
                this.jTextFieldBaseName.setText(this.spdHeader.getPackageName());
            }
            if (this.spdHeader.getGuidValue() != null) {
                this.jTextFieldGuid.setText(this.spdHeader.getGuidValue());
            }
            if (this.spdHeader.getVersion() != null) {
                this.jTextFieldVersion.setText(this.spdHeader.getVersion());
            }
            if (this.spdHeader.getLicense() != null) {
                this.jTextAreaLicense.setText(this.spdHeader.getLicense().getStringValue());
            }
            if (this.spdHeader.getCopyright() != null) {
                this.jTextFieldCopyright.setText(this.spdHeader.getCopyright());
            }
            if (this.spdHeader.getDescription() != null) {
                this.jTextAreaDescription.setText(this.spdHeader.getDescription());
            }
            if (this.spdHeader.getSpecificationArray(0) != null) {
                this.jTextFieldSpecification.setText(this.spdHeader.getSpecificationArray(0).toString());
            }
            if (this.spdHeader.getType() != null) {
                this.jComboBoxPackageType.setSelectedItem(this.spdHeader.getType().toString());
            }
            
            this.jComboBoxReadOnly.setSelectedItem(this.spdHeader.getReadOnly());
            this.jComboBoxRePackage.setSelectedItem(this.spdHeader.getRePackage());
        }
    }

    /**
     This method initializes jContentPane
     
     @return javax.swing.JPanel jContentPane
     
     **/
    private JPanel getJContentPane() {
        if (jContentPane == null) {
        	jContentPane = new JPanel();
            jContentPane.setLayout(null);
            jContentPane.setLocation(new java.awt.Point(0, 0));
            jContentPane.setSize(new java.awt.Dimension(500, 524));
            jLabelRePackage = new JLabel();
            jLabelRePackage.setBounds(new java.awt.Rectangle(15, 405, 140, 20));
            jLabelRePackage.setText("RePackage");
            jLabelReadOnly = new JLabel();
            jLabelReadOnly.setBounds(new java.awt.Rectangle(15, 380, 140, 20));
            jLabelReadOnly.setText("Read Only");
            jLabelPackageType = new JLabel();
            jLabelPackageType.setBounds(new java.awt.Rectangle(15, 355, 140, 20));
            jLabelPackageType.setText("Package Type");
            jLabelAbstract = new JLabel();
            jLabelAbstract.setBounds(new java.awt.Rectangle(15, 305, 140, 20));
            jLabelAbstract.setText("Abstract");
            jLabelSpecification = new JLabel();
            jLabelSpecification.setText("Specification");
            jLabelSpecification.setBounds(new java.awt.Rectangle(15, 280, 140, 20));
            jLabelDescription = new JLabel();
            jLabelDescription.setText("Description");
            jLabelDescription.setBounds(new java.awt.Rectangle(15, 195, 140, 20));
            jLabelCopyright = new JLabel();
            jLabelCopyright.setText("Copyright");
            jLabelCopyright.setBounds(new java.awt.Rectangle(15, 170, 140, 20));
            jLabelLicense = new JLabel();
            jLabelLicense.setText("License");
            jLabelLicense.setBounds(new java.awt.Rectangle(15, 85, 140, 20));
            jLabelVersion = new JLabel();
            jLabelVersion.setText("Version");
            jLabelVersion.setBounds(new java.awt.Rectangle(15, 60, 140, 20));
            jLabelGuid = new JLabel();
            jLabelGuid.setPreferredSize(new java.awt.Dimension(25, 15));
            jLabelGuid.setBounds(new java.awt.Rectangle(15, 35, 140, 20));
            jLabelGuid.setText("Guid");
            jLabelBaseName = new JLabel();
            jLabelBaseName.setText("Package Name");
            jLabelBaseName.setBounds(new java.awt.Rectangle(15, 10, 140, 20));
            jContentPane.add(jLabelBaseName, null);
            jContentPane.add(getJTextFieldBaseName(), null);
            jContentPane.add(jLabelGuid, null);
            jContentPane.add(getJTextFieldGuid(), null);
            jContentPane.add(jLabelVersion, null);
            jContentPane.add(getJTextFieldVersion(), null);
            jContentPane.add(getJButtonGenerateGuid(), null);
            jContentPane.add(jLabelLicense, null);
            jContentPane.add(jLabelCopyright, null);
            jContentPane.add(jLabelDescription, null);
            jContentPane.add(jLabelSpecification, null);
            jContentPane.add(getJTextFieldSpecification(), null);
            jContentPane.add(getJButtonOk(), null);
            jContentPane.add(getJButtonCancel(), null);
            jContentPane.add(getJScrollPaneLicense(), null);
            jContentPane.add(getJScrollPaneDescription(), null);
            jContentPane.add(jLabelAbstract, null);
            jContentPane.add(getJTextFieldAbstract(), null);
            jContentPane.add(jLabelPackageType, null);
            jContentPane.add(jLabelReadOnly, null);
            jContentPane.add(jLabelRePackage, null);
            jContentPane.add(getJComboBoxRePackage(), null);
            jContentPane.add(getJComboBoxReadOnly(), null);
            jContentPane.add(getJComboBoxPackageType(), null);

            jStarLabel1 = new StarLabel();
            jStarLabel1.setLocation(new java.awt.Point(0, 10));
            jStarLabel2 = new StarLabel();
            jStarLabel2.setLocation(new java.awt.Point(0, 35));
            jStarLabel3 = new StarLabel();
            jStarLabel3.setLocation(new java.awt.Point(0, 60));
            jStarLabel4 = new StarLabel();
            jStarLabel4.setLocation(new java.awt.Point(0, 85));
            jStarLabel5 = new StarLabel();
            jStarLabel5.setLocation(new java.awt.Point(0, 170));
            jStarLabel6 = new StarLabel();
            jStarLabel6.setLocation(new java.awt.Point(0, 195));
            jStarLabel7 = new StarLabel();
            jStarLabel7.setLocation(new java.awt.Point(0, 305));
            jStarLabel8 = new StarLabel();
            jStarLabel8.setLocation(new java.awt.Point(0, 355));
            jStarLabel9 = new StarLabel();
            jStarLabel9.setLocation(new java.awt.Point(0, 280));
            jStarLabel10 = new StarLabel();
            jStarLabel10.setLocation(new java.awt.Point(0,405));

            jStarLabel10.setVisible(false);
            jContentPane.add(jStarLabel1, null);
            jContentPane.add(jStarLabel2, null);
            jContentPane.add(jStarLabel3, null);
            jContentPane.add(jStarLabel4, null);
            jContentPane.add(jStarLabel5, null);
            jContentPane.add(jStarLabel6, null);
            jContentPane.add(jStarLabel7, null);
            jContentPane.add(jStarLabel8, null);
            jContentPane.add(jStarLabel9, null);
            jContentPane.add(jStarLabel10, null);
            jContentPane.add(getJTextFieldCopyright(), null);

        }
        return jContentPane;
    }

    /* (non-Javadoc)
     * @see java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
     *
     * Override actionPerformed to listen all actions
     *
     */
    public void actionPerformed(ActionEvent arg0) {
        if (arg0.getSource() == jButtonOk) {
            this.save();
            this.setEdited(true);
        }
        if (arg0.getSource() == jButtonCancel) {
            this.setEdited(false);
        }
        if (arg0.getSource() == jButtonGenerateGuid) {
            //ToDo: invoke GuidValueEditor
            jTextFieldGuid.setText(Tools.generateUuidString());
        }
    }

    /**
     Data validation for all fields
     
     @retval true - All datas are valid
     @retval false - At least one data is invalid
     
     **/
    public boolean check() {
        //
        // Check if all required fields are not empty
        //
        if (isEmpty(this.jTextFieldBaseName.getText())) {
            Log.err("Base Name couldn't be empty");
            return false;
        }
        if (isEmpty(this.jTextFieldGuid.getText())) {
            Log.err("Guid couldn't be empty");
            return false;
        }
        if (isEmpty(this.jTextFieldVersion.getText())) {
            Log.err("Version couldn't be empty");
            return false;
        }
        if (isEmpty(this.jTextAreaLicense.getText())) {
            Log.err("License couldn't be empty");
            return false;
        }
        if (isEmpty(this.jTextFieldCopyright.getText())) {
            Log.err("Copyright couldn't be empty");
            return false;
        }
        if (isEmpty(this.jTextAreaDescription.getText())) {
            Log.err("Description couldn't be empty");
            return false;
        }
        if (isEmpty(this.jTextFieldAbstract.getText())) {
            Log.err("Abstract couldn't be empty");
            return false;
        }

        //
        // Check if all fields have correct data types 
        //
        if (!DataValidation.isBaseName(this.jTextFieldBaseName.getText())) {
            Log.err("Incorrect data type for Base Name");
            return false;
        }
        if (!DataValidation.isGuid((this.jTextFieldGuid).getText())) {
            Log.err("Incorrect data type for Guid");
            return false;
        }
        if (!DataValidation.isAbstract(this.jTextFieldAbstract.getText())) {
            Log.err("Incorrect data type for Abstract");
            return false;
        }
        if (!DataValidation.isCopyright(this.jTextFieldCopyright.getText())) {
            Log.err("Incorrect data type for Copyright");
            return false;
        }
        return true;
    }

    /**
     Save all components of Spd Header
     if exists spdHeader, set the value directly
     if not exists spdHeader, new an instance first
     
     **/
    public void save() {
        try {
            if (this.spdHeader == null) {
                spdHeader = SpdHeaderDocument.SpdHeader.Factory.newInstance();
            }
            if (!isEmpty(this.jTextFieldBaseName.getText())) {
                this.spdHeader.setPackageName(this.jTextFieldBaseName.getText());
            }
            if (!isEmpty(this.jTextFieldGuid.getText())) {
            	this.spdHeader.setGuidValue(this.jTextFieldGuid.getText());
            }

            this.spdHeader.setVersion(this.jTextFieldVersion.getText());

            if (this.spdHeader.getLicense() != null) {
                this.spdHeader.getLicense().setStringValue(this.jTextAreaLicense.getText());
            } else {
                LicenseDocument.License mLicense = LicenseDocument.License.Factory.newInstance();
                mLicense.setStringValue(this.jTextAreaLicense.getText());
                this.spdHeader.setLicense(mLicense);
            }

            this.spdHeader.setCopyright(this.jTextFieldCopyright.getText());
            this.spdHeader.setDescription(this.jTextAreaDescription.getText());

            if (this.spdHeader.getAbstract() != null) {
                this.spdHeader.setAbstract(this.jTextFieldAbstract.getText());
            }
            if (this.spdHeader.getSpecificationArray(0) != null) {
                
                this.spdHeader.getSpecificationArray(0).setStringValue(this.jTextFieldSpecification.getText());
            } else {
                SpecificationDocument.Specification mSpecification = SpecificationDocument.Specification.Factory
                                                                                                                .newInstance();
                mSpecification.setStringValue(this.jTextFieldSpecification.getText());
                this.spdHeader.setSpecificationArray(0, mSpecification);
            }
            this.spdHeader.setType(PackageType.Enum.forString(this.jComboBoxPackageType.getSelectedItem().toString()));
            this.spdHeader.setReadOnly(new Boolean(this.jComboBoxReadOnly.getSelectedItem().toString()));
            this.spdHeader.setRePackage(new Boolean(this.jComboBoxRePackage.getSelectedItem().toString()));
            
            if (this.spdHeader.getCreatedDate() == null) {
                this.spdHeader.setCreatedDate(Tools.getCurrentDateTime());
            }
            
            this.spdHeader.setModifiedDate(Tools.getCurrentDateTime());
        } catch (Exception e) {
            Log.err("Save Package", e.getMessage());
        }
    }

    /**
     This method initializes Package type and Compontent type
     
     **/
    private void initFrame() {
        jComboBoxPackageType.addItem("SOURCE");
        jComboBoxPackageType.addItem("BINARY");
        jComboBoxPackageType.addItem("MIXED");
      

        jComboBoxReadOnly.addItem("TRUE");
        jComboBoxReadOnly.addItem("FALSE");
        
        jComboBoxRePackage.addItem("FALSE");
        jComboBoxRePackage.addItem("TRUE");
        
    }

    /**
     Get spdHeaderDocument.spdHeader
     
     @return spdHeaderDocument.spdHeader
     
     **/
    public SpdHeaderDocument.SpdHeader getspdHeader() {
        return spdHeader;
    }

    /**
     Set spdHeaderDocument.spdHeader
     
     @param spdHeader The input data of spdHeaderDocument.spdHeader
     
     **/
    public void setSpdHeader(SpdHeaderDocument.SpdHeader spdHeader) {
        this.spdHeader = spdHeader;
    }
    
	/* (non-Javadoc)
	 * @see java.awt.event.ComponentListener#componentResized(java.awt.event.ComponentEvent)
	 * 
	 * Override componentResized to resize all components when frame's size is changed
	 */
	public void componentResized(ComponentEvent arg0) {
		resizeComponentWidth(this.jTextFieldBaseName, this.getWidth());
		resizeComponentWidth(this.jTextFieldGuid, this.getWidth());
		resizeComponentWidth(this.jTextFieldVersion, this.getWidth());
		resizeComponentWidth(this.jScrollPaneLicense, this.getWidth());
		resizeComponentWidth(this.jTextFieldCopyright, this.getWidth());
		resizeComponentWidth(this.jScrollPaneDescription, this.getWidth());
		resizeComponentWidth(this.jTextFieldSpecification, this.getWidth());
		resizeComponentWidth(this.jTextFieldAbstract, this.getWidth());
		resizeComponentWidth(this.jComboBoxPackageType, this.getWidth());
		resizeComponentWidth(this.jComboBoxReadOnly, this.getWidth());
        resizeComponentWidth(this.jComboBoxRePackage, this.getWidth());
		relocateComponentX(this.jButtonGenerateGuid, this.getWidth(), DataType.SPACE_TO_RIGHT_FOR_GENERATE_BUTTON);
	}
}
