/** @file
 
 The file is used to create, update FpdHeader of Fpd file
 
 Copyright (c) 2006, Intel Corporation
 All rights reserved. This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution.  The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php
 
 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 
 **/

package org.tianocore.frameworkwizard.platform.ui;

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


import org.tianocore.LicenseDocument;

import org.tianocore.PlatformHeaderDocument;
import org.tianocore.SpecificationDocument;

import org.tianocore.frameworkwizard.common.DataType;
import org.tianocore.frameworkwizard.common.DataValidation;
import org.tianocore.frameworkwizard.common.Log;
import org.tianocore.frameworkwizard.common.Tools;
import org.tianocore.frameworkwizard.common.ui.IInternalFrame;
import org.tianocore.frameworkwizard.common.ui.StarLabel;

/**
 The class is used to create, update FpdHeader of Fpd file
 It extends IInternalFrame
 
 @since PackageEditor 1.0

 **/
public class FpdHeader extends IInternalFrame {

    ///
    /// Define class Serial Version UID
    ///
    private static final long serialVersionUID = -8152099582923006900L;

    //
    //Define class members
    //
    private JPanel jContentPane = null;  //  @jve:decl-index=0:visual-constraint="10,53"

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

    private StarLabel jStarLabel1 = null;

    private StarLabel jStarLabel2 = null;

    private StarLabel jStarLabel3 = null;

    private StarLabel jStarLabel4 = null;

    private StarLabel jStarLabel5 = null;

    private StarLabel jStarLabel6 = null;

    private StarLabel jStarLabel7 = null;

    private StarLabel jStarLabel8 = null;

    private StarLabel jStarLabel9 = null;
    
    private PlatformHeaderDocument.PlatformHeader fpdHeader = null;

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
            jButtonOk.setBounds(new java.awt.Rectangle(290,351,90,20));
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
            jButtonCancel.setBounds(new java.awt.Rectangle(390,351,90,20));
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
        new FpdHeader().setVisible(true);
    }

    /**
     This is the default constructor
     
     **/
    public FpdHeader() {
        super();
        init();
        this.setVisible(true);
    }

    /**
     This is the override edit constructor
     
     @param inFpdHeader The input data of FpdHeaderDocument.FpdHeader
     
     **/
    public FpdHeader(PlatformHeaderDocument.PlatformHeader inFpdHeader) {
        super();
        init(inFpdHeader);
        this.setVisible(true);
        
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
     
     @param inFpdHeader  The input data of FpdHeaderDocument.FpdHeader
     
     **/
    private void init(PlatformHeaderDocument.PlatformHeader inFpdHeader) {
        init();
        if (inFpdHeader != null) {
            setFpdHeader(inFpdHeader);
            if (this.fpdHeader.getPlatformName() != null) {
                this.jTextFieldBaseName.setText(this.fpdHeader.getPlatformName());
            }
            if (this.fpdHeader.getGuidValue() != null) {
                this.jTextFieldGuid.setText(this.fpdHeader.getGuidValue());
            }
            if (this.fpdHeader.getVersion() != null) {
                this.jTextFieldVersion.setText(this.fpdHeader.getVersion());
            }
            if (this.fpdHeader.getLicense() != null) {
                this.jTextAreaLicense.setText(this.fpdHeader.getLicense().getStringValue());
            }
            if (this.fpdHeader.getCopyright() != null) {
                this.jTextFieldCopyright.setText(this.fpdHeader.getCopyright());
            }
            if (this.fpdHeader.getDescription() != null) {
                this.jTextAreaDescription.setText(this.fpdHeader.getDescription());
            }
            if (this.fpdHeader.getSpecification() != null) {
                this.jTextFieldSpecification.setText(this.fpdHeader.getSpecification().toString());
            }
            
            
            
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
            jContentPane.setSize(new java.awt.Dimension(500,423));
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
            jLabelBaseName.setText("Platform Name");
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
            jStarLabel8.setVisible(false);
            jStarLabel9 = new StarLabel();
            jStarLabel9.setLocation(new java.awt.Point(0, 280));
            jContentPane.add(jStarLabel1, null);
            jContentPane.add(jStarLabel2, null);
            jContentPane.add(jStarLabel3, null);
            jContentPane.add(jStarLabel4, null);
            jContentPane.add(jStarLabel5, null);
            jContentPane.add(jStarLabel6, null);
            jContentPane.add(jStarLabel7, null);
            jContentPane.add(jStarLabel8, null);
            jContentPane.add(jStarLabel9, null);
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
     Save all components of Fpd Header
     if exists FpdHeader, set the value directly
     if not exists FpdHeader, new an instance first
     
     **/
    public void save() {
        try {
            if (this.fpdHeader == null) {
                fpdHeader =PlatformHeaderDocument.PlatformHeader.Factory.newInstance();
            }
            if (!isEmpty(this.jTextFieldBaseName.getText())) {
                this.fpdHeader.setPlatformName(this.jTextFieldBaseName.getText());
            }
            if (!isEmpty(this.jTextFieldGuid.getText())) {
            	this.fpdHeader.setGuidValue(this.jTextFieldGuid.getText());
            }

            this.fpdHeader.setVersion(this.jTextFieldVersion.getText());

            if (this.fpdHeader.getLicense() != null) {
                this.fpdHeader.getLicense().setStringValue(this.jTextAreaLicense.getText());
            } else {
                LicenseDocument.License mLicense = LicenseDocument.License.Factory.newInstance();
                mLicense.setStringValue(this.jTextAreaLicense.getText());
                this.fpdHeader.setLicense(mLicense);
            }

            this.fpdHeader.setCopyright(this.jTextFieldCopyright.getText());
            this.fpdHeader.setDescription(this.jTextAreaDescription.getText());

            if (this.fpdHeader.getAbstract() != null) {
                this.fpdHeader.setAbstract(this.jTextFieldAbstract.getText());
            }
            if (this.fpdHeader.getSpecification() != null) {
                
                this.fpdHeader.getSpecification().setStringValue(this.jTextFieldSpecification.getText());
            } else {
                SpecificationDocument.Specification mSpecification = SpecificationDocument.Specification.Factory
                                                                                                                .newInstance();
                mSpecification.setStringValue(this.jTextFieldSpecification.getText());
                this.fpdHeader.setSpecification( mSpecification);
            }
            
            if (this.fpdHeader.getCreatedDate() == null) {
                this.fpdHeader.setCreatedDate(Tools.getCurrentDateTime());
            }
            
            this.fpdHeader.setModifiedDate(Tools.getCurrentDateTime());
        } catch (Exception e) {
            Log.err("Save Package", e.getMessage());
        }
    }

    /**
     This method initializes Package type and Compontent type
     
     **/
    private void initFrame() {
        
        
    }

    /**
     Get FpdHeaderDocument.FpdHeader
     
     @return FpdHeaderDocument.FpdHeader
     
     **/
    public PlatformHeaderDocument.PlatformHeader getFpdHeader() {
        return fpdHeader;
    }

    /**
     Set FpdHeaderDocument.FpdHeader
     
     @param FpdHeader The input data of FpdHeaderDocument.FpdHeader
     
     **/
    public void setFpdHeader(PlatformHeaderDocument.PlatformHeader fpdHeader) {
        this.fpdHeader = fpdHeader;
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
		
		relocateComponentX(this.jButtonGenerateGuid, this.getWidth(), DataType.SPACE_TO_RIGHT_FOR_GENERATE_BUTTON);
	}
}
