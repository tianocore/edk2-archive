/** @file
 
 The file is used to create, update BuildOptions of Fpd file
 
 Copyright (c) 2006, Intel Corporation
 All rights reserved. This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution.  The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php
 
 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 
 **/
package org.tianocore.frameworkwizard.platform.ui;

import java.awt.BorderLayout;
import java.awt.Dimension;

import javax.swing.JPanel;
import javax.swing.JDialog;
import javax.swing.JTabbedPane;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import java.awt.FlowLayout;
import javax.swing.JTextField;
import javax.swing.JButton;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JComboBox;
import javax.swing.JRadioButton;
import javax.swing.table.DefaultTableModel;
import javax.swing.JList;

import org.tianocore.frameworkwizard.common.ui.IInternalFrame;

public class FpdBuildOptions extends IInternalFrame {

    private JPanel jContentPane = null;
    private JPanel jPanel = null;
    private JPanel jPanel1 = null;
    private JPanel jPanel2 = null;
    private JPanel jPanel3 = null;
    private JTabbedPane jTabbedPane = null;
    private JPanel jPanel4 = null;
    private JPanel jPanel5 = null;
    private JLabel jLabel = null;
    private JCheckBox jCheckBox = null;
    private JCheckBox jCheckBox1 = null;
    private JCheckBox jCheckBox2 = null;
    private JPanel jPanel6 = null;
    private JPanel jPanel7 = null;
    private JLabel jLabel1 = null;
    private JTextField jTextField = null;
    private JButton jButton = null;
    private JButton jButton1 = null;
    private JScrollPane jScrollPane = null;
    private JTable jTable = null;
    private JLabel jLabel2 = null;
    private JCheckBox jCheckBox3 = null;
    private JComboBox jComboBox = null;
    private JTextField jTextField1 = null;
    private JButton jButton2 = null;
    private JButton jButton3 = null;
    private JScrollPane jScrollPane1 = null;
    private JTable jTable1 = null;
    private JPanel jPanel8 = null;
    private JPanel jPanel9 = null;
    private JPanel jPanel10 = null;
    private JPanel jPanel11 = null;
    private JRadioButton jRadioButton = null;
    private JTextField jTextField2 = null;
    private JLabel jLabel3 = null;
    private JTextField jTextField3 = null;
    private JButton jButton4 = null;
    private JButton jButton5 = null;
    private JRadioButton jRadioButton1 = null;
    private JLabel jLabel4 = null;
    private JTextField jTextField4 = null;
    private JScrollPane jScrollPane2 = null;
    private JTable jTable2 = null;
    private JPanel jPanel12 = null;
    private JLabel jLabel5 = null;
    private JTextField jTextField5 = null;
    private JCheckBox jCheckBox4 = null;
    private JComboBox jComboBox1 = null;
    private JButton jButton6 = null;
    private JButton jButton7 = null;
    private JScrollPane jScrollPane3 = null;
    private JTable jTable3 = null;
    private DefaultTableModel fileNameTableModel = null;
    private DefaultTableModel imageEntryPointTableModel = null;
    private DefaultTableModel outputDirectoryTableModel = null;
    private DefaultTableModel antTaskTableModel = null;
    private DefaultTableModel ffsAttributesTableModel = null;
    private DefaultTableModel optionsTableModel = null;
    private JPanel jPanel13 = null;
    private JPanel jPanel14 = null;
    private JPanel jPanel18 = null;
    private JScrollPane jScrollPane4 = null;
    private JLabel jLabel6 = null;
    private JList jList = null;
    private JPanel jPanel15 = null;
    private JPanel jPanel16 = null;
    private JPanel jPanel17 = null;
    private JLabel jLabel7 = null;
    private JTextField jTextField6 = null;
    private JButton jButton8 = null;
    private JButton jButton9 = null;
    private JCheckBox jCheckBox5 = null;
    private JScrollPane jScrollPane5 = null;
    private JTable jTable4 = null;
    private JPanel jPanel19 = null;
    private DynamicTree dt = null;
    private JLabel jLabel8 = null;
    private JPanel jPanel20 = null;
    private JLabel jLabel9 = null;
    private JTextField jTextField7 = null;
    private JLabel jLabel10 = null;
    private JComboBox jComboBox2 = null;
    private JLabel jLabel11 = null;
    private JComboBox jComboBox3 = null;
    private JLabel jLabel12 = null;
    private JTextField jTextField8 = null;
    private JScrollPane jScrollPane6 = null;
    private JTable jTable5 = null;
    private JButton jButton10 = null;
    private JButton jButton11 = null;
    /**
     * This method initializes jPanel	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel() {
        if (jPanel == null) {
            jPanel = new JPanel();
        }
        return jPanel;
    }

    /**
     * This method initializes jPanel1	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel1() {
        if (jPanel1 == null) {
            jPanel1 = new JPanel();
        }
        return jPanel1;
    }

    /**
     * This method initializes jPanel2	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel2() {
        if (jPanel2 == null) {
            jPanel2 = new JPanel();
        }
        return jPanel2;
    }

    /**
     * This method initializes jPanel3	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel3() {
        if (jPanel3 == null) {
            jPanel3 = new JPanel();
        }
        return jPanel3;
    }

    /**
     * This method initializes jTabbedPane	
     * 	
     * @return javax.swing.JTabbedPane	
     */
    private JTabbedPane getJTabbedPane() {
        if (jTabbedPane == null) {
            jTabbedPane = new JTabbedPane();
            jTabbedPane.addTab("General", null, getJPanel4(), null);
            jTabbedPane.addTab("File Names", null, getJPanel12(), null);
            jTabbedPane.addTab("FFS", null, getJPanel13(), null);
            jTabbedPane.addTab("Options", null, getJPanel20(), null);
            jTabbedPane.addTab("User Defined ANT Tasks", null, getJPanel8(), null);
        }
        return jTabbedPane;
    }

    /**
     * This method initializes jPanel4	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel4() {
        if (jPanel4 == null) {
            jPanel4 = new JPanel();
            jPanel4.setLayout(new BorderLayout());
            jPanel4.add(getJPanel5(), java.awt.BorderLayout.NORTH);
            jPanel4.add(getJPanel6(), java.awt.BorderLayout.SOUTH);
            jPanel4.add(getJPanel7(), java.awt.BorderLayout.CENTER);
        }
        return jPanel4;
    }

    /**
     * This method initializes jPanel5	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel5() {
        if (jPanel5 == null) {
            FlowLayout flowLayout = new FlowLayout();
            flowLayout.setHgap(20);
            flowLayout.setAlignment(java.awt.FlowLayout.LEFT);
            jLabel = new JLabel();
            jLabel.setText("Tool Chain Families");
            jPanel5 = new JPanel();
            jPanel5.setLayout(flowLayout);
            jPanel5.add(jLabel, null);
            jPanel5.add(getJCheckBox(), null);
            jPanel5.add(getJCheckBox1(), null);
            jPanel5.add(getJCheckBox2(), null);
        }
        return jPanel5;
    }

    /**
     * This method initializes jCheckBox	
     * 	
     * @return javax.swing.JCheckBox	
     */
    private JCheckBox getJCheckBox() {
        if (jCheckBox == null) {
            jCheckBox = new JCheckBox();
            jCheckBox.setText("MSFT");
        }
        return jCheckBox;
    }

    /**
     * This method initializes jCheckBox1	
     * 	
     * @return javax.swing.JCheckBox	
     */
    private JCheckBox getJCheckBox1() {
        if (jCheckBox1 == null) {
            jCheckBox1 = new JCheckBox();
            jCheckBox1.setText("GCC");
        }
        return jCheckBox1;
    }

    /**
     * This method initializes jCheckBox2	
     * 	
     * @return javax.swing.JCheckBox	
     */
    private JCheckBox getJCheckBox2() {
        if (jCheckBox2 == null) {
            jCheckBox2 = new JCheckBox();
            jCheckBox2.setText("INTC");
        }
        return jCheckBox2;
    }

    /**
     * This method initializes jPanel6	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel6() {
        if (jPanel6 == null) {
            FlowLayout flowLayout2 = new FlowLayout();
            flowLayout2.setAlignment(java.awt.FlowLayout.LEFT);
            flowLayout2.setHgap(20);
            jLabel2 = new JLabel();
            jLabel2.setText("Output Directory");
            jPanel6 = new JPanel();
            jPanel6.setLayout(flowLayout2);
            jPanel6.setPreferredSize(new java.awt.Dimension(10,140));
            jPanel6.add(jLabel2, null);
            jPanel6.add(getJCheckBox3(), null);
            jPanel6.add(getJComboBox(), null);
            jPanel6.add(getJTextField1(), null);
            jPanel6.add(getJButton2(), null);
            jPanel6.add(getJButton3(), null);
            jPanel6.add(getJScrollPane1(), null);
        }
        return jPanel6;
    }

    /**
     * This method initializes jPanel7	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel7() {
        if (jPanel7 == null) {
            FlowLayout flowLayout1 = new FlowLayout();
            flowLayout1.setHgap(20);
            flowLayout1.setAlignment(java.awt.FlowLayout.LEFT);
            jLabel1 = new JLabel();
            jLabel1.setText("Image Entry Point");
            jLabel1.setPreferredSize(new java.awt.Dimension(109,16));
            jPanel7 = new JPanel();
            jPanel7.setPreferredSize(new java.awt.Dimension(972,100));
            jPanel7.setLayout(flowLayout1);
            jPanel7.add(jLabel1, null);
            jPanel7.add(getJTextField(), null);
            jPanel7.add(getJButton(), null);
            jPanel7.add(getJButton1(), null);
            jPanel7.add(getJScrollPane(), null);
        }
        return jPanel7;
    }

    /**
     * This method initializes jTextField	
     * 	
     * @return javax.swing.JTextField	
     */
    private JTextField getJTextField() {
        if (jTextField == null) {
            jTextField = new JTextField();
            jTextField.setPreferredSize(new java.awt.Dimension(150,20));
        }
        return jTextField;
    }

    /**
     * This method initializes jButton	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton() {
        if (jButton == null) {
            jButton = new JButton();
            jButton.setPreferredSize(new java.awt.Dimension(70,20));
            jButton.setText("Add");
        }
        return jButton;
    }

    /**
     * This method initializes jButton1	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton1() {
        if (jButton1 == null) {
            jButton1 = new JButton();
            jButton1.setPreferredSize(new java.awt.Dimension(70,20));
            jButton1.setText("Delete");
        }
        return jButton1;
    }

    /**
     * This method initializes jScrollPane	
     * 	
     * @return javax.swing.JScrollPane	
     */
    private JScrollPane getJScrollPane() {
        if (jScrollPane == null) {
            jScrollPane = new JScrollPane();
            jScrollPane.setPreferredSize(new java.awt.Dimension(453,100));
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
            imageEntryPointTableModel = new DefaultTableModel();
            jTable = new JTable(imageEntryPointTableModel);
            imageEntryPointTableModel.addColumn("ImageEntryPoint");
        }
        return jTable;
    }

    /**
     * This method initializes jCheckBox3	
     * 	
     * @return javax.swing.JCheckBox	
     */
    private JCheckBox getJCheckBox3() {
        if (jCheckBox3 == null) {
            jCheckBox3 = new JCheckBox();
            jCheckBox3.setText("Intermediate Directories");
        }
        return jCheckBox3;
    }

    /**
     * This method initializes jComboBox	
     * 	
     * @return javax.swing.JComboBox	
     */
    private JComboBox getJComboBox() {
        if (jComboBox == null) {
            jComboBox = new JComboBox();
            jComboBox.setPreferredSize(new java.awt.Dimension(100,20));
            jComboBox.addItem("MODULE");
            jComboBox.addItem("UNIFIED");
            jComboBox.setSelectedIndex(0);
        }
        return jComboBox;
    }

    /**
     * This method initializes jTextField1	
     * 	
     * @return javax.swing.JTextField	
     */
    private JTextField getJTextField1() {
        if (jTextField1 == null) {
            jTextField1 = new JTextField();
            jTextField1.setPreferredSize(new java.awt.Dimension(280,20));
        }
        return jTextField1;
    }

    /**
     * This method initializes jButton2	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton2() {
        if (jButton2 == null) {
            jButton2 = new JButton();
            jButton2.setPreferredSize(new java.awt.Dimension(70,20));
            jButton2.setText("Add");
        }
        return jButton2;
    }

    /**
     * This method initializes jButton3	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton3() {
        if (jButton3 == null) {
            jButton3 = new JButton();
            jButton3.setPreferredSize(new java.awt.Dimension(70,20));
            jButton3.setText("Delete");
        }
        return jButton3;
    }

    /**
     * This method initializes jScrollPane1	
     * 	
     * @return javax.swing.JScrollPane	
     */
    private JScrollPane getJScrollPane1() {
        if (jScrollPane1 == null) {
            jScrollPane1 = new JScrollPane();
            jScrollPane1.setPreferredSize(new java.awt.Dimension(453,100));
            jScrollPane1.setViewportView(getJTable1());
        }
        return jScrollPane1;
    }

    /**
     * This method initializes jTable1	
     * 	
     * @return javax.swing.JTable	
     */
    private JTable getJTable1() {
        if (jTable1 == null) {
            outputDirectoryTableModel = new DefaultTableModel();
            jTable1 = new JTable(outputDirectoryTableModel);
            outputDirectoryTableModel.addColumn("Output Directory");
            outputDirectoryTableModel.addColumn("Intermediate Directories");
        }
        return jTable1;
    }

    /**
     * This method initializes jPanel8	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel8() {
        if (jPanel8 == null) {
            jPanel8 = new JPanel();
            jPanel8.setLayout(new BorderLayout());
            jPanel8.add(getJPanel9(), java.awt.BorderLayout.NORTH);
            jPanel8.add(getJPanel10(), java.awt.BorderLayout.SOUTH);
            jPanel8.add(getJPanel11(), java.awt.BorderLayout.CENTER);
        }
        return jPanel8;
    }

    /**
     * This method initializes jPanel9	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel9() {
        if (jPanel9 == null) {
            jLabel3 = new JLabel();
            jLabel3.setText("ID");
            jPanel9 = new JPanel();
            jPanel9.add(getJRadioButton(), null);
            jPanel9.add(getJTextField2(), null);
            jPanel9.add(jLabel3, null);
            jPanel9.add(getJTextField3(), null);
            jPanel9.add(getJButton4(), null);
            jPanel9.add(getJButton5(), null);
        }
        return jPanel9;
    }

    /**
     * This method initializes jPanel10	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel10() {
        if (jPanel10 == null) {
            jPanel10 = new JPanel();
        }
        return jPanel10;
    }

    /**
     * This method initializes jPanel11	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel11() {
        if (jPanel11 == null) {
            FlowLayout flowLayout3 = new FlowLayout();
            flowLayout3.setHgap(20);
            jLabel4 = new JLabel();
            jLabel4.setText("Execution Order");
            jLabel4.setPreferredSize(new java.awt.Dimension(100,16));
            jPanel11 = new JPanel();
            jPanel11.setLayout(flowLayout3);
            jPanel11.add(getJRadioButton1(), null);
            jPanel11.add(jLabel4, null);
            jPanel11.add(getJTextField4(), null);
            jPanel11.add(getJScrollPane2(), null);
        }
        return jPanel11;
    }

    /**
     * This method initializes jRadioButton	
     * 	
     * @return javax.swing.JRadioButton	
     */
    private JRadioButton getJRadioButton() {
        if (jRadioButton == null) {
            jRadioButton = new JRadioButton();
            jRadioButton.setText("ANT tasks File");
        }
        return jRadioButton;
    }

    /**
     * This method initializes jTextField2	
     * 	
     * @return javax.swing.JTextField	
     */
    private JTextField getJTextField2() {
        if (jTextField2 == null) {
            jTextField2 = new JTextField();
            jTextField2.setPreferredSize(new java.awt.Dimension(100,20));
        }
        return jTextField2;
    }

    /**
     * This method initializes jTextField3	
     * 	
     * @return javax.swing.JTextField	
     */
    private JTextField getJTextField3() {
        if (jTextField3 == null) {
            jTextField3 = new JTextField();
            jTextField3.setPreferredSize(new java.awt.Dimension(40,20));
        }
        return jTextField3;
    }

    /**
     * This method initializes jButton4	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton4() {
        if (jButton4 == null) {
            jButton4 = new JButton();
            jButton4.setPreferredSize(new java.awt.Dimension(70,20));
            jButton4.setText("Add");
        }
        return jButton4;
    }

    /**
     * This method initializes jButton5	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton5() {
        if (jButton5 == null) {
            jButton5 = new JButton();
            jButton5.setPreferredSize(new java.awt.Dimension(70,20));
            jButton5.setText("Delete");
        }
        return jButton5;
    }

    /**
     * This method initializes jRadioButton1	
     * 	
     * @return javax.swing.JRadioButton	
     */
    private JRadioButton getJRadioButton1() {
        if (jRadioButton1 == null) {
            jRadioButton1 = new JRadioButton();
            jRadioButton1.setText("ANT Command");
            jRadioButton1.setPreferredSize(new java.awt.Dimension(180,24));
        }
        return jRadioButton1;
    }

    /**
     * This method initializes jTextField4	
     * 	
     * @return javax.swing.JTextField	
     */
    private JTextField getJTextField4() {
        if (jTextField4 == null) {
            jTextField4 = new JTextField();
            jTextField4.setPreferredSize(new java.awt.Dimension(100,20));
        }
        return jTextField4;
    }

    /**
     * This method initializes jScrollPane2	
     * 	
     * @return javax.swing.JScrollPane	
     */
    private JScrollPane getJScrollPane2() {
        if (jScrollPane2 == null) {
            jScrollPane2 = new JScrollPane();
            jScrollPane2.setPreferredSize(new java.awt.Dimension(453,100));
            jScrollPane2.setViewportView(getJTable2());
        }
        return jScrollPane2;
    }

    /**
     * This method initializes jTable2	
     * 	
     * @return javax.swing.JTable	
     */
    private JTable getJTable2() {
        if (jTable2 == null) {
            antTaskTableModel = new DefaultTableModel();
            jTable2 = new JTable(antTaskTableModel);
            antTaskTableModel.addColumn("ID");
        }
        return jTable2;
    }

    /**
     * This method initializes jPanel12	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel12() {
        if (jPanel12 == null) {
            FlowLayout flowLayout4 = new FlowLayout();
            flowLayout4.setHgap(5);
            flowLayout4.setAlignment(java.awt.FlowLayout.RIGHT);
            jLabel5 = new JLabel();
            jLabel5.setText("File Name");
            jLabel5.setName("JLabel");
            jPanel12 = new JPanel();
            jPanel12.setLayout(flowLayout4);
            jPanel12.add(jLabel5, null);
            jPanel12.add(getJTextField5(), null);
            jPanel12.add(getJCheckBox4(), null);
            jPanel12.add(getJComboBox1(), null);
            jPanel12.add(getJButton6(), null);
            jPanel12.add(getJScrollPane3(), null);
            jPanel12.add(getJButton7(), null);
        }
        return jPanel12;
    }

    /**
     * This method initializes jTextField5	
     * 	
     * @return javax.swing.JTextField	
     */
    private JTextField getJTextField5() {
        if (jTextField5 == null) {
            jTextField5 = new JTextField();
            jTextField5.setPreferredSize(new java.awt.Dimension(150,20));
        }
        return jTextField5;
    }

    /**
     * This method initializes jCheckBox4	
     * 	
     * @return javax.swing.JCheckBox	
     */
    private JCheckBox getJCheckBox4() {
        if (jCheckBox4 == null) {
            jCheckBox4 = new JCheckBox();
            jCheckBox4.setText("Arch");
        }
        return jCheckBox4;
    }

    /**
     * This method initializes jComboBox1	
     * 	
     * @return javax.swing.JComboBox	
     */
    private JComboBox getJComboBox1() {
        if (jComboBox1 == null) {
            jComboBox1 = new JComboBox();
            jComboBox1.setPreferredSize(new java.awt.Dimension(80,20));
            jComboBox1.addItem("ALL");
            jComboBox1.addItem("ARM");
            jComboBox1.addItem("EBC");
            jComboBox1.addItem("IA32");
            jComboBox1.addItem("IPF");
            jComboBox1.addItem("PPC");
            jComboBox1.addItem("X64");
            jComboBox1.setSelectedIndex(0);
        }
        return jComboBox1;
    }

    /**
     * This method initializes jButton6	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton6() {
        if (jButton6 == null) {
            jButton6 = new JButton();
            jButton6.setPreferredSize(new java.awt.Dimension(70,20));
            jButton6.setText("Add");
        }
        return jButton6;
    }

    /**
     * This method initializes jButton7	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton7() {
        if (jButton7 == null) {
            jButton7 = new JButton();
            jButton7.setPreferredSize(new java.awt.Dimension(70,20));
            jButton7.setText("Delete");
        }
        return jButton7;
    }

    /**
     * This method initializes jScrollPane3	
     * 	
     * @return javax.swing.JScrollPane	
     */
    private JScrollPane getJScrollPane3() {
        if (jScrollPane3 == null) {
            jScrollPane3 = new JScrollPane();
            jScrollPane3.setPreferredSize(new java.awt.Dimension(453,150));
            jScrollPane3.setViewportView(getJTable3());
        }
        return jScrollPane3;
    }

    /**
     * This method initializes jTable3	
     * 	
     * @return javax.swing.JTable	
     */
    private JTable getJTable3() {
        if (jTable3 == null) {
            fileNameTableModel = new DefaultTableModel();
            jTable3 = new JTable(fileNameTableModel);
            fileNameTableModel.addColumn("FileName");
            fileNameTableModel.addColumn("Arch");
        }
        return jTable3;
    }

    /**
     * This method initializes jPanel13	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel13() {
        if (jPanel13 == null) {
            jPanel13 = new JPanel();
            jPanel13.setLayout(new BorderLayout());
            jPanel13.add(getJPanel14(), java.awt.BorderLayout.WEST);
            jPanel13.add(getJPanel18(), java.awt.BorderLayout.CENTER);
        }
        return jPanel13;
    }

    /**
     * This method initializes jPanel14	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel14() {
        if (jPanel14 == null) {
            jLabel6 = new JLabel();
            jLabel6.setText("FFS Types");
            jPanel14 = new JPanel();
            jPanel14.setPreferredSize(new java.awt.Dimension(120,300));
            jPanel14.add(jLabel6, null);
            jPanel14.add(getJScrollPane4(), null);
        }
        return jPanel14;
    }

    /**
     * This method initializes jPanel18	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel18() {
        if (jPanel18 == null) {
            jPanel18 = new JPanel();
            jPanel18.setLayout(new BorderLayout());
            jPanel18.add(getJPanel15(), java.awt.BorderLayout.NORTH);
            jPanel18.add(getJPanel16(), java.awt.BorderLayout.SOUTH);
            jPanel18.add(getJPanel17(), java.awt.BorderLayout.EAST);
            jPanel18.add(getJPanel19(), java.awt.BorderLayout.CENTER);
        }
        return jPanel18;
    }

    /**
     * This method initializes jScrollPane4	
     * 	
     * @return javax.swing.JScrollPane	
     */
    private JScrollPane getJScrollPane4() {
        if (jScrollPane4 == null) {
            jScrollPane4 = new JScrollPane();
            jScrollPane4.setPreferredSize(new java.awt.Dimension(120,280));
            jScrollPane4.setViewportView(getJList());
        }
        return jScrollPane4;
    }

    /**
     * This method initializes jList	
     * 	
     * @return javax.swing.JList	
     */
    private JList getJList() {
        if (jList == null) {
            jList = new JList();
        }
        return jList;
    }

    /**
     * This method initializes jPanel15	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel15() {
        if (jPanel15 == null) {
            FlowLayout flowLayout5 = new FlowLayout();
            flowLayout5.setAlignment(java.awt.FlowLayout.RIGHT);
            jLabel7 = new JLabel();
            jLabel7.setText("Type");
            jPanel15 = new JPanel();
            jPanel15.setLayout(flowLayout5);
            jPanel15.add(jLabel7, null);
            jPanel15.add(getJTextField6(), null);
            jPanel15.add(getJButton8(), null);
            jPanel15.add(getJButton9(), null);
        }
        return jPanel15;
    }

    /**
     * This method initializes jPanel16	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel16() {
        if (jPanel16 == null) {
            FlowLayout flowLayout6 = new FlowLayout();
            flowLayout6.setHgap(5);
            flowLayout6.setAlignment(java.awt.FlowLayout.LEFT);
            jPanel16 = new JPanel();
            jPanel16.setPreferredSize(new java.awt.Dimension(491,130));
            jPanel16.setLayout(flowLayout6);
            jPanel16.add(getJCheckBox5(), null);
            jPanel16.add(getJScrollPane5(), null);
        }
        return jPanel16;
    }

    /**
     * This method initializes jPanel17	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel17() {
        if (jPanel17 == null) {
            jPanel17 = new JPanel();
            dt = new DynamicTree();
            dt.setPreferredSize(new Dimension(100, 140));
            jPanel17.add(dt, null);
        }
        return jPanel17;
    }

    /**
     * This method initializes jTextField6	
     * 	
     * @return javax.swing.JTextField	
     */
    private JTextField getJTextField6() {
        if (jTextField6 == null) {
            jTextField6 = new JTextField();
            jTextField6.setPreferredSize(new java.awt.Dimension(150,20));
        }
        return jTextField6;
    }

    /**
     * This method initializes jButton8	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton8() {
        if (jButton8 == null) {
            jButton8 = new JButton();
            jButton8.setPreferredSize(new java.awt.Dimension(70,20));
            jButton8.setText("Add");
        }
        return jButton8;
    }

    /**
     * This method initializes jButton9	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton9() {
        if (jButton9 == null) {
            jButton9 = new JButton();
            jButton9.setPreferredSize(new java.awt.Dimension(70,20));
            jButton9.setText("Delete");
        }
        return jButton9;
    }

    /**
     * This method initializes jCheckBox5	
     * 	
     * @return javax.swing.JCheckBox	
     */
    private JCheckBox getJCheckBox5() {
        if (jCheckBox5 == null) {
            jCheckBox5 = new JCheckBox();
            jCheckBox5.setText("Attributes");
            jCheckBox5.setPreferredSize(new java.awt.Dimension(81,20));
        }
        return jCheckBox5;
    }

    /**
     * This method initializes jScrollPane5	
     * 	
     * @return javax.swing.JScrollPane	
     */
    private JScrollPane getJScrollPane5() {
        if (jScrollPane5 == null) {
            jScrollPane5 = new JScrollPane();
            jScrollPane5.setPreferredSize(new java.awt.Dimension(350,100));
            jScrollPane5.setViewportView(getJTable4());
        }
        return jScrollPane5;
    }

    /**
     * This method initializes jTable4	
     * 	
     * @return javax.swing.JTable	
     */
    private JTable getJTable4() {
        if (jTable4 == null) {
            ffsAttributesTableModel = new DefaultTableModel();
            jTable4 = new JTable(ffsAttributesTableModel);
            ffsAttributesTableModel.addColumn("Name");
            ffsAttributesTableModel.addColumn("Value");
        }
        return jTable4;
    }

    /**
     * This method initializes jPanel19	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel19() {
        if (jPanel19 == null) {
            jLabel8 = new JLabel();
            jLabel8.setText("Section Attributes Here");
            jPanel19 = new JPanel();
            jPanel19.add(jLabel8, null);
        }
        return jPanel19;
    }

    /**
     * This method initializes jPanel20	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel20() {
        if (jPanel20 == null) {
            jLabel12 = new JLabel();
            jLabel12.setText("Command Code");
            jLabel11 = new JLabel();
            jLabel11.setText("Arch");
            jLabel10 = new JLabel();
            jLabel10.setText("Tool Chain Family");
            jLabel9 = new JLabel();
            jLabel9.setText("Option Contents");
            jPanel20 = new JPanel();
            jPanel20.add(jLabel9, null);
            jPanel20.add(getJTextField7(), null);
            jPanel20.add(jLabel10, null);
            jPanel20.add(getJComboBox2(), null);
            jPanel20.add(jLabel11, null);
            jPanel20.add(getJComboBox3(), null);
            jPanel20.add(jLabel12, null);
            jPanel20.add(getJTextField8(), null);
            jPanel20.add(getJScrollPane6(), null);
            jPanel20.add(getJButton10(), null);
            jPanel20.add(getJButton11(), null);
        }
        return jPanel20;
    }

    /**
     * This method initializes jTextField7	
     * 	
     * @return javax.swing.JTextField	
     */
    private JTextField getJTextField7() {
        if (jTextField7 == null) {
            jTextField7 = new JTextField();
            jTextField7.setPreferredSize(new java.awt.Dimension(380,20));
        }
        return jTextField7;
    }

    /**
     * This method initializes jComboBox2	
     * 	
     * @return javax.swing.JComboBox	
     */
    private JComboBox getJComboBox2() {
        if (jComboBox2 == null) {
            jComboBox2 = new JComboBox();
            jComboBox2.setPreferredSize(new java.awt.Dimension(80,20));
            jComboBox2.addItem("MSFT");
            jComboBox2.addItem("GCC");
            jComboBox2.addItem("CYGWIN");
            jComboBox2.addItem("INTEL");
            jComboBox2.setSelectedIndex(0);
        }
        return jComboBox2;
    }

    /**
     * This method initializes jComboBox3	
     * 	
     * @return javax.swing.JComboBox	
     */
    private JComboBox getJComboBox3() {
        if (jComboBox3 == null) {
            jComboBox3 = new JComboBox();
            jComboBox3.setPreferredSize(new java.awt.Dimension(80,20));
            jComboBox3.addItem("ALL");
            jComboBox3.addItem("ARM");
            jComboBox3.addItem("EBC");
            jComboBox3.addItem("IA32");
            jComboBox3.addItem("IPF");
            jComboBox3.addItem("PPC");
            jComboBox3.addItem("X64");
            jComboBox3.setSelectedIndex(0);
        }
        return jComboBox3;
    }

    /**
     * This method initializes jTextField8	
     * 	
     * @return javax.swing.JTextField	
     */
    private JTextField getJTextField8() {
        if (jTextField8 == null) {
            jTextField8 = new JTextField();
            jTextField8.setPreferredSize(new java.awt.Dimension(70,20));
        }
        return jTextField8;
    }

    /**
     * This method initializes jScrollPane6	
     * 	
     * @return javax.swing.JScrollPane	
     */
    private JScrollPane getJScrollPane6() {
        if (jScrollPane6 == null) {
            jScrollPane6 = new JScrollPane();
            jScrollPane6.setPreferredSize(new java.awt.Dimension(453,200));
            jScrollPane6.setViewportView(getJTable5());
        }
        return jScrollPane6;
    }

    /**
     * This method initializes jTable5	
     * 	
     * @return javax.swing.JTable	
     */
    private JTable getJTable5() {
        if (jTable5 == null) {
            optionsTableModel = new DefaultTableModel();
            jTable5 = new JTable(optionsTableModel);
            optionsTableModel.addColumn("ToolChainFamily");
            optionsTableModel.addColumn("Arch");
            optionsTableModel.addColumn("CommandCode");
            optionsTableModel.addColumn("Contents");
        }
        return jTable5;
    }

    /**
     * This method initializes jButton10	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton10() {
        if (jButton10 == null) {
            jButton10 = new JButton();
            jButton10.setText("Add");
            jButton10.setPreferredSize(new java.awt.Dimension(70,20));
        }
        return jButton10;
    }

    /**
     * This method initializes jButton11	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton11() {
        if (jButton11 == null) {
            jButton11 = new JButton();
            jButton11.setText("Delete");
            jButton11.setPreferredSize(new java.awt.Dimension(70,20));
        }
        return jButton11;
    }

    /**
     * @param args
     */
    public static void main(String[] args) {
        // TODO Auto-generated method stub
        new FpdBuildOptions().setVisible(true);
    }

    /**
     * This is the default constructor
     */
    public FpdBuildOptions() {
        super();
        initialize();
        this.setVisible(true);
    }

    /**
     * This method initializes this
     * 
     * @return void
     */
    private void initialize() {
        this.setSize(521, 386);
        this.setContentPane(getJContentPane());
    }

    /**
     * This method initializes jContentPane
     * 
     * @return javax.swing.JPanel
     */
    private JPanel getJContentPane() {
        if (jContentPane == null) {
            jContentPane = new JPanel();
            jContentPane.setLayout(new BorderLayout());
            jContentPane.add(getJPanel(), java.awt.BorderLayout.SOUTH);
            jContentPane.add(getJPanel1(), java.awt.BorderLayout.NORTH);
            jContentPane.add(getJPanel2(), java.awt.BorderLayout.WEST);
            jContentPane.add(getJPanel3(), java.awt.BorderLayout.EAST);
            jContentPane.add(getJTabbedPane(), java.awt.BorderLayout.CENTER);
        }
        return jContentPane;
    }

}  //  @jve:decl-index=0:visual-constraint="10,10"
