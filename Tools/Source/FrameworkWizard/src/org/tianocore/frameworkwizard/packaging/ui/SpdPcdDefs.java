/** @file
  Java class SpdPcdDefs is GUI for create PCD definition elements of spd file.
 
Copyright (c) 2006, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/
package org.tianocore.frameworkwizard.packaging.ui;

import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.ButtonGroup;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JLabel;
import javax.swing.JTextField;
import javax.swing.JComboBox;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.table.DefaultTableModel;

import org.tianocore.frameworkwizard.common.Tools;
import org.tianocore.frameworkwizard.common.ui.IInternalFrame;
import org.tianocore.frameworkwizard.common.ui.StarLabel;
import javax.swing.JScrollPane;
import javax.swing.JTable;

/**
 GUI for create PCD definition elements of spd file
  
 @since PackageEditor 1.0
**/
public class SpdPcdDefs extends IInternalFrame {

    private JPanel jContentPane = null;  //  @jve:decl-index=0:visual-constraint="87,10"

    private JLabel jLabelItemType = null;

    private JLabel jLabelC_Name = null;

    private JTextField jTextFieldC_Name = null;

    private JLabel jLabelToken = null;

    private JTextField jTextFieldToken = null;

    private JLabel jLabelDataType = null;

    private JButton jButtonOk = null;

    private JButton jButtonCancel = null;

    private JComboBox jComboBoxDataType = null;

    private JLabel jLabelOverrideID = null;

    private StarLabel jStarLabel2 = null;

    private StarLabel jStarLabel3 = null;

    private StarLabel jStarLabel4 = null;

    private StarLabel jStarLabel = null;

    private StarLabel jStarLabel1 = null;

    private JLabel jLabelTokenSpace = null;

    private JTextField jTextFieldTsGuid = null;

    private JLabel jLabelVarVal = null;

    private JTextField jTextFieldVarVal = null;

    private JTextField jTextField = null;

    private JLabel jLabelDefVal = null;

    private JTextField jTextFieldDefaultValue = null;

    private JButton jButtonAdd = null;
    
    private PcdDefTableModel model = null;

    private JButton jButtonRemove = null;

    private JButton jButtonClearAll = null;

    private JScrollPane jScrollPane = null;

    private JTable jTable = null;

    private JButton jButtonGen = null;
    
    private GenGuidDialog guidDialog = null;

    /**
     This method initializes this
     
     **/
    private void initialize() {
        this.setTitle("PCD Definition");
        this.setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);

    }

    /**
     This method initializes jTextFieldC_Name	
     	
     @return javax.swing.JTextField	
     **/
    private JTextField getJTextFieldC_Name() {
        if (jTextFieldC_Name == null) {
            jTextFieldC_Name = new JTextField();
            jTextFieldC_Name.setBounds(new java.awt.Rectangle(156,9,320,20));
        }
        return jTextFieldC_Name;
    }

    /**
     This method initializes jTextFieldToken	
     	
     @return javax.swing.JTextField	
     **/
    private JTextField getJTextFieldToken() {
        if (jTextFieldToken == null) {
            jTextFieldToken = new JTextField();
            jTextFieldToken.setBounds(new java.awt.Rectangle(157,33,318,20));
        }
        return jTextFieldToken;
    }

    /**
     This method initializes jButtonOk	
     	
     @return javax.swing.JButton	
     **/
    private JButton getJButtonOk() {
        if (jButtonOk == null) {
            jButtonOk = new JButton();
            jButtonOk.setText("OK");
            jButtonOk.setBounds(new java.awt.Rectangle(279,247,90,20));
            jButtonOk.addActionListener(this);
        }
        return jButtonOk;
    }

    /**
     This method initializes jButtonCancel	
     	
     @return javax.swing.JButton	
     **/
    private JButton getJButtonCancel() {
        if (jButtonCancel == null) {
            jButtonCancel = new JButton();
            jButtonCancel.setText("Cancel");
            jButtonCancel.setBounds(new java.awt.Rectangle(389,247,90,20));
            jButtonCancel.addActionListener(this);
        }
        return jButtonCancel;
    }

    /**
     This method initializes jComboBoxDataType	
     	
     @return javax.swing.JComboBox	
     **/
    private JComboBox getJComboBoxDataType() {
        if (jComboBoxDataType == null) {
            jComboBoxDataType = new JComboBox();
            jComboBoxDataType.setBounds(new java.awt.Rectangle(157,84,110,20));
        }
        return jComboBoxDataType;
    }

    /**
     This is the default constructor
     **/
    public SpdPcdDefs() {
        super();
        init();
        initialize();
        
    }

    /**
     This method initializes this
     
     @return void
     **/
    private void init() {
        this.setSize(500, 650);
        this.setContentPane(getJContentPane());
        this.setTitle("Add PCDs");
        
        this.getRootPane().setDefaultButton(jButtonOk);
        initFrame();
        this.setVisible(true);
    }

    private JPanel getJContentPane() {
  		if (jContentPane == null) {	
            jLabelOverrideID.setBounds(new java.awt.Rectangle(14,197,140,20));
            jLabelOverrideID.setText("Default Value");
            jLabelDefVal = new JLabel();
            jLabelDefVal.setBounds(new java.awt.Rectangle(278,109,80,20));
            jLabelDefVal.setText("Default Value");
            jLabelVarVal = new JLabel();
            jLabelVarVal.setBounds(new java.awt.Rectangle(278,84,84,20));
            jLabelVarVal.setText("Variable Value");
            jLabelC_Name = new JLabel();
            jLabelC_Name.setText("C_Name");
            jLabelC_Name.setBounds(new java.awt.Rectangle(11,9,140,20));
            jLabelTokenSpace = new JLabel();
            jLabelTokenSpace.setBounds(new java.awt.Rectangle(12,58,140,20));
 
            jLabelTokenSpace.setBounds(new java.awt.Rectangle(17,60,134,16));
            jLabelTokenSpace.setText("Token Space");
            jLabelDataType = new JLabel();
            jLabelDataType.setText("Data Type");
            jLabelDataType.setBounds(new java.awt.Rectangle(11,83,140,20));
            jLabelToken = new JLabel();
            jLabelToken.setText("Token");
            jLabelToken.setBounds(new java.awt.Rectangle(12,33,140,20));
            jLabelItemType = new JLabel();
            jLabelItemType.setText("Data Offset");
            jLabelItemType.setBounds(new java.awt.Rectangle(11,108,140,20));
            
            
            jContentPane = new JPanel();
            jContentPane.setLayout(null);
            jContentPane.setSize(new java.awt.Dimension(479,310));
            
            jContentPane.add(jLabelItemType, null);
            jContentPane.add(jLabelC_Name, null);
            jContentPane.add(jLabelTokenSpace, null);
            jContentPane.add(getJTextFieldTsGuid(), null);
            jContentPane.add(jLabelVarVal, null);
            jContentPane.add(getJTextFieldVarVal(), null);
            jContentPane.add(getJTextFieldC_Name(), null);
            jContentPane.add(jLabelToken, null);
            jContentPane.add(getJTextFieldToken(), null);
            jContentPane.add(jLabelDataType, null);
            jContentPane.add(getJButtonOk(), null);
            jContentPane.add(getJButtonCancel(), null);
            jContentPane.add(getJComboBoxDataType(), null);
            jContentPane.add(jLabelOverrideID, null);
            jStarLabel = new StarLabel();
            jStarLabel1 = new StarLabel();
            jStarLabel1.setBounds(new java.awt.Rectangle(2,8,10,20));
            jStarLabel2 = new StarLabel();
            jStarLabel3 = new StarLabel();
            jStarLabel4 = new StarLabel();
            jStarLabel.setLocation(new java.awt.Point(2,84));
            jStarLabel4.setLocation(new java.awt.Point(2, 109));
            jStarLabel2.setLocation(new java.awt.Point(2,33));
            jStarLabel3.setLocation(new java.awt.Point(2, 58));
            jContentPane.add(jStarLabel2, null);
            jContentPane.add(jStarLabel3, null);
            jContentPane.add(jStarLabel, null);
            jContentPane.add(jStarLabel1, null);
            jContentPane.add(jStarLabel4, null);
            jContentPane.add(getJTextField(), null);
            jContentPane.add(jLabelDefVal, null);
            jContentPane.add(getJTextFieldDefaultValue(), null);
            jContentPane.add(getJButtonAdd(), null);
            jContentPane.add(getJButtonRemove(), null);
            jContentPane.add(getJButtonClearAll(), null);
            jContentPane.add(getJScrollPane(), null);
            jContentPane.add(getJButtonGen(), null);
        }
        return jContentPane;
    }

    /**
     This method initializes comboboxes			
 			jContentPane.add(jLabelTokenSpace, null);
    
     **/
    private void initFrame() {

        jComboBoxDataType.addItem("UINT8");
        jComboBoxDataType.addItem("UINT16");
        jComboBoxDataType.addItem("UINT32");
        jComboBoxDataType.addItem("UINT64");
        jComboBoxDataType.addItem("VOID*");
        jComboBoxDataType.addItem("BOOLEAN");
    }

    public void actionPerformed(ActionEvent arg0) {
        
            if (arg0.getSource() == jButtonOk) {
                this.save();
                this.dispose();

            }
            if (arg0.getSource() == jButtonCancel) {
                this.dispose();
            }

            if (arg0.getSource() == jButtonAdd) {
                
                
                //ToDo: check before add
                String[] row = {"", "", "", "", ""};
                row[4] = jTextField.getText();
                row[3] = jComboBoxDataType.getSelectedItem().toString();
                row[2] = jTextFieldTsGuid.getText();
                row[1] = jTextFieldToken.getText();
                row[0] = jTextFieldC_Name.getText();
                model.addRow(row);
            }
            //
            // remove selected line
            //
            if (arg0.getSource() == jButtonRemove) {
                int rowSelected = jTable.getSelectedRow();
                if (rowSelected >= 0) {
                    model.removeRow(rowSelected);
                }
            }

            if (arg0.getSource() == jButtonClearAll) {
                if (model.getRowCount() == 0) {
                    return;
                }
                for (int i = 0; i < model.getRowCount(); ++i) {
                    model.removeRow(i);
                }
            }
            
            if (arg0.getSource() == jButtonGen) {
                guidDialog = new GenGuidDialog(this);
                guidDialog.setGuid(jTextFieldTsGuid.getText());
            }
            
            if (arg0.getActionCommand().equals("GenGuidValue")) {
                jTextFieldTsGuid.setText(guidDialog.getGuid());
            }
            
        

        

    }

    protected void save() {
        
    }

    /**
     * This method initializes jTextFieldTsGuid	
     * 	
     * @return javax.swing.JTextField	
     */
    private JTextField getJTextFieldTsGuid() {
        if (jTextFieldTsGuid == null) {
            jTextFieldTsGuid = new JTextField();
            jTextFieldTsGuid.setBounds(new java.awt.Rectangle(158,58,249,20));
        }
        return jTextFieldTsGuid;
    }

    /**
     * This method initializes jTextFieldVarVal	
     * 	
     * @return javax.swing.JTextField	
     */
    private JTextField getJTextFieldVarVal() {
        if (jTextFieldVarVal == null) {
            jTextFieldVarVal = new JTextField();
            jTextFieldVarVal.setBounds(new java.awt.Rectangle(369,84,104,20));
        }
        return jTextFieldVarVal;
    }

    /**
     * This method initializes jTextField	
     * 	
     * @return javax.swing.JTextField	
     */
    private JTextField getJTextField() {
        if (jTextField == null) {
            jTextField = new JTextField();
            jTextField.setBounds(new java.awt.Rectangle(158,108,109,20));
        }
        return jTextField;
    }

    /**
     * This method initializes jTextFieldDefaultValue	
     * 	
     * @return javax.swing.JTextField	
     */
    private JTextField getJTextFieldDefaultValue() {
        if (jTextFieldDefaultValue == null) {
            jTextFieldDefaultValue = new JTextField();
            jTextFieldDefaultValue.setBounds(new java.awt.Rectangle(369,109,104,20));
        }
        return jTextFieldDefaultValue;
    }

    /**
     * This method initializes jButtonAdd	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButtonAdd() {
        if (jButtonAdd == null) {
            jButtonAdd = new JButton();
            jButtonAdd.setBounds(new java.awt.Rectangle(195,136,71,20));
            jButtonAdd.setText("Add");
        }
        return jButtonAdd;   
        
    }   
    
    class PcdDefTableModel extends DefaultTableModel {
        public boolean isCellEditable(){
            return false;
        }
    }

    /**
     * This method initializes jButtonRemove	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButtonRemove() {
        if (jButtonRemove == null) {
            jButtonRemove = new JButton();
            jButtonRemove.setBounds(new java.awt.Rectangle(278,136,81,20));
            jButtonRemove.setText("Remove");
        }
        return jButtonRemove;
    }

    /**
     * This method initializes jButtonClearAll	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButtonClearAll() {
        if (jButtonClearAll == null) {
            jButtonClearAll = new JButton();
            jButtonClearAll.setBounds(new java.awt.Rectangle(369,136,90,20));
            jButtonClearAll.setText("Clear All");
        }
        return jButtonClearAll;
    }

    /**
     * This method initializes jScrollPane	
     * 	
     * @return javax.swing.JScrollPane	
     */
    private JScrollPane getJScrollPane() {
        if (jScrollPane == null) {
            jScrollPane = new JScrollPane();
            jScrollPane.setBounds(new java.awt.Rectangle(8,165,465,80));
            jScrollPane.setViewportView(getJTable());
        }
        return jScrollPane;
    }

    /**
     * This method initializes jTable	
     * 	
     * @return javax.swing.JTable	
     */
    private JTable getJTable() {
        if (jTable == null) {
            model = new PcdDefTableModel();
            jTable = new JTable(model);
            jTable.setRowHeight(20);
            model.addColumn("Token");
            model.addColumn("C_Name");
            model.addColumn("TokenSpace");
            model.addColumn("DatumType");
            model.addColumn("DataOffset");
        }
        return jTable;
    }

    /**
     * This method initializes jButtonGen	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButtonGen() {
        if (jButtonGen == null) {
            jButtonGen = new JButton();
            jButtonGen.setBounds(new java.awt.Rectangle(413,58,58,20));
            jButtonGen.setText("Gen");
        }
        return jButtonGen;
    }
} //  @jve:decl-index=0:visual-constraint="22,11"
