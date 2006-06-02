/** @file
  Java class FpdFlash is GUI for Flash element operation in SPD file.
 
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
import javax.swing.JPanel;

import javax.swing.ButtonGroup;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JTabbedPane;
import javax.swing.JButton;

import org.tianocore.frameworkwizard.common.ui.IInternalFrame;
import org.tianocore.frameworkwizard.common.ui.StarLabel;


import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;

import javax.swing.JCheckBox;
import javax.swing.JTextField;
import java.awt.GridLayout;
import javax.swing.JLabel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JList;
import javax.swing.table.DefaultTableModel;
import javax.swing.JComboBox;
import java.awt.Dimension;
import javax.swing.JRadioButton;
import javax.swing.JTextArea;
import java.awt.CardLayout;

public class FpdFlash extends IInternalFrame {

    static JFrame frame;
    private JPanel jContentPane = null;
    private JPanel jPanel = null;
    private JPanel jPanel1 = null;
    private JPanel jPanel2 = null;
    private JPanel jPanel3 = null;
    private JTabbedPane jTabbedPane = null;
    private JPanel jPanelFvImages = null;
    private JPanel jPanelFlashDef = null;
    private JPanel jPanelMicrocodeFile = null;
    private JButton jButtonOk = null;
    private JButton jButtonCancel = null;
    private JCheckBox jCheckBox = null;
    private JTextField jTextFieldMicrocodeFile = null;
    private JButton jButtonBrowse = null;
    private JPanel jPanelFvImageN = null;
    private JPanel jPanelFvImageW = null;
    private JPanel jPanelFvImageS = null;
    private JCheckBox jCheckBox1 = null;
    private JLabel jLabel = null;
    private JTextField jTextField = null;
    private JLabel jLabel1 = null;
    private JTextField jTextField1 = null;
    private JButton jButton = null;
    private JScrollPane jScrollPane = null;
    private JScrollPane jScrollPane1 = null;
    private JTable jTable = null;
    private JPanel jPanel4 = null;
    private JButton jButton1 = null;
    private DefaultTableModel fvPropertyTableModel = null;
    private DefaultTableModel fvImageNameTableModel = null;
    private DefaultTableModel fvImageParaTableModel = null;
    private JPanel jPanelFvImageC = null;
    private JCheckBox jCheckBox2 = null;
    private JLabel jLabel2 = null;
    private JComboBox jComboBox = null;
    private JLabel jLabel3 = null;
    private JLabel jLabel4 = null;
    private JTextField jTextField2 = null;
    private JButton jButton2 = null;
    private JScrollPane jScrollPane2 = null;
    private JTable jTable1 = null;
    private JButton jButton3 = null;
    private JPanel jPanel5 = null;
    private JButton jButton4 = null;
    private JScrollPane jScrollPane3 = null;
    private JTable jTable2 = null;
    private JButton jButton6 = null;
    private JCheckBox jCheckBox3 = null;
    private JLabel jLabel5 = null;
    private JComboBox jComboBox1 = null;
    private JCheckBox jCheckBox4 = null;
    private JCheckBox jCheckBox5 = null;
    private JCheckBox jCheckBox6 = null;
    private JCheckBox jCheckBox7 = null;
    private JCheckBox jCheckBox8 = null;
    private JCheckBox jCheckBox9 = null;
    private JCheckBox jCheckBox10 = null;
    private JCheckBox jCheckBox11 = null;
    private JCheckBox jCheckBox12 = null;
    private JCheckBox jCheckBox13 = null;
    private JPanel jPanel6 = null;
    private JPanel jPanelFdfN = null;
    private JPanel jPanelFdfW = null;
    private JPanel jPanelFdfS = null;
    private JRadioButton jRadioButton = null;
    private JTextField jTextField3 = null;
    private JPanel jPanel9 = null;
    private JPanel jPanel8 = null;
    private JButton jButton5 = null;
    private JRadioButton jRadioButton1 = null;
    private JPanel jPanel10 = null;
    private JPanel jPanel11 = null;
    private JScrollPane jScrollPane4 = null;
    private JTable jTable3 = null;
    private JCheckBox jCheckBox14 = null;
    private JLabel jLabel6 = null;
    private JTextField jTextField4 = null;
    private JLabel jLabel7 = null;
    private JTextField jTextField5 = null;
    private JLabel jLabel8 = null;
    private JTextField jTextField6 = null;
    private JComboBox jComboBox2 = null;
    private JLabel jLabel9 = null;
    private JComboBox jComboBox3 = null;
    private JButton jButton7 = null;
    private JButton jButton8 = null;
    private DefaultTableModel fdfImageDefTableModel = null;
    private DefaultTableModel fdfBlocksTableModel = null;
    private DefaultTableModel fdfRegionsTableModel = null;
    private DefaultTableModel fdfSubRegionsTableModel = null;
    
    private JLabel jLabel10 = null;
    private JScrollPane jScrollPane5 = null;
    private JTextArea jTextArea = null;
    private JLabel jLabel11 = null;
    private JTextField jTextField7 = null;
    private JLabel jLabel12 = null;
    private JCheckBox jCheckBox15 = null;
    private JScrollPane jScrollPane6 = null;
    private JList jList = null;
    private JTabbedPane jTabbedPane1 = null;
    private JPanel jPanelDevInfoBase = null;
    private JPanel jPanelDevInfoBlocks = null;
    private JPanel jPanelDevInfoRegions = null;
    private JLabel jLabel13 = null;
    private JTextField jTextField8 = null;
    private JLabel jLabel14 = null;
    private JTextField jTextField9 = null;
    private JLabel jLabel15 = null;
    private JTextField jTextField10 = null;
    private JLabel jLabel16 = null;
    private JComboBox jComboBox4 = null;
    private JButton jButton9 = null;
    private JButton jButton10 = null;
    private JScrollPane jScrollPane7 = null;
    private JTable jTable4 = null;
    private JButton jButton11 = null;
    private JButton jButton12 = null;
    private JLabel jLabel17 = null;
    private JScrollPane jScrollPane8 = null;
    private JTable jTable5 = null;
    private JLabel jLabel18 = null;
    private JScrollPane jScrollPane9 = null;
    private JTable jTable6 = null;
    private JButton jButton13 = null;
    private JButton jButton14 = null;
    private JPanel jPanel7 = null;
    private JPanel jPanel12 = null;
    private JPanel jPanel13 = null;
    private JPanel jPanel14 = null;
    
    public FpdFlash() {
        super();
        // TODO Auto-generated constructor stub

        initialize();
        this.setVisible(true);
    }

    
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
            FlowLayout flowLayout = new FlowLayout();
            flowLayout.setAlignment(java.awt.FlowLayout.RIGHT);
            flowLayout.setHgap(15);
            jPanel1 = new JPanel();
            jPanel1.setLayout(flowLayout);
            jPanel1.setComponentOrientation(java.awt.ComponentOrientation.LEFT_TO_RIGHT);
            jPanel1.add(getJButtonCancel(), null);
            jPanel1.add(getJButtonOk(), null);
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
            jTabbedPane.addTab("FV Images", null, getJPanelFvImages(), null);
            jTabbedPane.addTab("Flash Definition", null, getJPanelFlashDef(), null);
            jTabbedPane.addTab("Microcode File", null, getJPanelMicrocodeFile(), null);
            ButtonGroup bg = new ButtonGroup();
            bg.add(jRadioButton);
            bg.add(jRadioButton1);
        }
        return jTabbedPane;
    }

    /**
     * This method initializes jPanelFvImages	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanelFvImages() {
        if (jPanelFvImages == null) {
            jPanelFvImages = new JPanel();
            jPanelFvImages.setLayout(new BorderLayout());
            jPanelFvImages.add(getJPanelFvImageN(), java.awt.BorderLayout.NORTH);
            jPanelFvImages.add(getJPanelFvImageW(), java.awt.BorderLayout.WEST);
            jPanelFvImages.add(getJPanelFvImageS(), java.awt.BorderLayout.SOUTH);
            jPanelFvImages.add(getJPanelFvImageC(), java.awt.BorderLayout.CENTER);
            
        }
        return jPanelFvImages;
    }

    /**
     * This method initializes jPanelFlashDef	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanelFlashDef() {
        if (jPanelFlashDef == null) {
            jPanelFlashDef = new JPanel();
            jPanelFlashDef.setLayout(new BorderLayout());
            jPanelFlashDef.add(getJPanelFdfN(), java.awt.BorderLayout.NORTH);
            jPanelFlashDef.add(getJPanelFdfW(), java.awt.BorderLayout.WEST);
            jPanelFlashDef.add(getJPanelFdfS(), java.awt.BorderLayout.SOUTH);
            jPanelFlashDef.add(getJTabbedPane1(), java.awt.BorderLayout.CENTER);
        }
        return jPanelFlashDef;
    }

    /**
     * This method initializes jPanelMicrocodeFile	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanelMicrocodeFile() {
        if (jPanelMicrocodeFile == null) {
            FlowLayout flowLayout1 = new FlowLayout();
            flowLayout1.setAlignment(java.awt.FlowLayout.LEFT);
            jPanelMicrocodeFile = new JPanel();
            jPanelMicrocodeFile.setLayout(flowLayout1);
            jPanelMicrocodeFile.add(getJCheckBox(), null);
            jPanelMicrocodeFile.add(getJTextFieldMicrocodeFile(), null);
            jPanelMicrocodeFile.add(getJButtonBrowse(), null);
        }
        return jPanelMicrocodeFile;
    }

    /**
     * This method initializes jButtonOk	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButtonOk() {
        if (jButtonOk == null) {
            jButtonOk = new JButton();
            jButtonOk.setPreferredSize(new java.awt.Dimension(80,20));
            jButtonOk.setText("Cancel");
        }
        return jButtonOk;
    }

    /**
     * This method initializes jButtonCancel	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButtonCancel() {
        if (jButtonCancel == null) {
            jButtonCancel = new JButton();
            jButtonCancel.setPreferredSize(new java.awt.Dimension(80,20));
            jButtonCancel.setText("Ok");
        }
        return jButtonCancel;
    }

    /**
     * This method initializes jCheckBox	
     * 	
     * @return javax.swing.JCheckBox	
     */
    private JCheckBox getJCheckBox() {
        if (jCheckBox == null) {
            jCheckBox = new JCheckBox();
            jCheckBox.setText("Microcode File");
            jCheckBox.addItemListener(new ItemListener() {

                public void itemStateChanged(ItemEvent arg0) {
                    // TODO Auto-generated method stub
                    if (jCheckBox.isSelected()){
                        jTextFieldMicrocodeFile.setEnabled(true);
                        jButtonBrowse.setEnabled(true);
                    }
                    else {
                        
                        jTextFieldMicrocodeFile.setEnabled(false);
                        jButtonBrowse.setEnabled(false);
                    }
                }

                
                
            });
        }
        return jCheckBox;
    }


    /**
     * This method initializes jTextFieldMicrocodeFile	
     * 	
     * @return javax.swing.JTextField	
     */
    private JTextField getJTextFieldMicrocodeFile() {
        if (jTextFieldMicrocodeFile == null) {
            jTextFieldMicrocodeFile = new JTextField();
            jTextFieldMicrocodeFile.setPreferredSize(new java.awt.Dimension(300,20));
            jTextFieldMicrocodeFile.setEnabled(false);
        }
        return jTextFieldMicrocodeFile;
    }


    /**
     * This method initializes jButtonBrowse	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButtonBrowse() {
        if (jButtonBrowse == null) {
            jButtonBrowse = new JButton();
            jButtonBrowse.setText("Browse");
            jButtonBrowse.setEnabled(false);
            jButtonBrowse.setPreferredSize(new java.awt.Dimension(78,20));
            jButtonBrowse.addMouseListener(new MouseAdapter(){

                @Override
                public void mouseClicked(MouseEvent arg0) {
                    // TODO Auto-generated method stub
                    JFileChooser chooser = new JFileChooser();
                    chooser.setMultiSelectionEnabled(false);
                    chooser.setFileSelectionMode(JFileChooser.FILES_AND_DIRECTORIES);
                    int retval = chooser.showOpenDialog(frame);
                    if (retval == JFileChooser.APPROVE_OPTION) {

                        File theFile = chooser.getSelectedFile();
                        jTextFieldMicrocodeFile.setText(theFile.getPath());
                    }
                }
                
            });
        }
        return jButtonBrowse;
    }


    /**
     * This method initializes jPanelFvImageN	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanelFvImageN() {
        if (jPanelFvImageN == null) {
            jLabel1 = new JLabel();
            jLabel1.setText("Value");
            jLabel1.setPreferredSize(new java.awt.Dimension(38,20));
            jLabel = new JLabel();
            jLabel.setText("Name");
            jLabel.setPreferredSize(new java.awt.Dimension(38,20));
            FlowLayout flowLayout2 = new FlowLayout();
            flowLayout2.setAlignment(java.awt.FlowLayout.CENTER);
            flowLayout2.setHgap(15);
            jPanelFvImageN = new JPanel();
            jPanelFvImageN.setPreferredSize(new java.awt.Dimension(576,100));
            jPanelFvImageN.setBorder(javax.swing.BorderFactory.createBevelBorder(javax.swing.border.BevelBorder.RAISED));
            jPanelFvImageN.setLayout(flowLayout2);
            jPanelFvImageN.add(getJCheckBox1(), null);
            jPanelFvImageN.add(jLabel, null);
            jPanelFvImageN.add(getJTextField(), null);
            jPanelFvImageN.add(jLabel1, null);
            jPanelFvImageN.add(getJTextField1(), null);
            jPanelFvImageN.add(getJScrollPane1(), null);
            jPanelFvImageN.add(getJPanel4(), null);
        }
        return jPanelFvImageN;
    }


    /**
     * This method initializes jPanelFvImageW	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanelFvImageW() {
        if (jPanelFvImageW == null) {
            jPanelFvImageW = new JPanel();
            jPanelFvImageW.setPreferredSize(new java.awt.Dimension(10,2));
        }
        return jPanelFvImageW;
    }


    /**
     * This method initializes jPanelFvImageS	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanelFvImageS() {
        if (jPanelFvImageS == null) {
            FlowLayout flowLayout4 = new FlowLayout();
            flowLayout4.setAlignment(java.awt.FlowLayout.RIGHT);
            jPanelFvImageS = new JPanel();
            jPanelFvImageS.setLayout(flowLayout4);
            jPanelFvImageS.add(getJButton6(), null);
        }
        return jPanelFvImageS;
    }


    /**
     * This method initializes jCheckBox1	
     * 	
     * @return javax.swing.JCheckBox	
     */
    private JCheckBox getJCheckBox1() {
        if (jCheckBox1 == null) {
            jCheckBox1 = new JCheckBox();
            jCheckBox1.setText("FV Properties");
        }
        return jCheckBox1;
    }


    /**
     * This method initializes jTextField	
     * 	
     * @return javax.swing.JTextField	
     */
    private JTextField getJTextField() {
        if (jTextField == null) {
            jTextField = new JTextField();
            jTextField.setPreferredSize(new java.awt.Dimension(100,20));
        }
        return jTextField;
    }


    /**
     * This method initializes jTextField1	
     * 	
     * @return javax.swing.JTextField	
     */
    private JTextField getJTextField1() {
        if (jTextField1 == null) {
            jTextField1 = new JTextField();
            jTextField1.setPreferredSize(new java.awt.Dimension(100,20));
        }
        return jTextField1;
    }


    /**
     * This method initializes jButton	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton() {
        if (jButton == null) {
            jButton = new JButton();
            jButton.setPreferredSize(new java.awt.Dimension(80,20));
            jButton.setText("Add");
        }
        return jButton;
    }


    /**
     * This method initializes jScrollPane1	
     * 	
     * @return javax.swing.JScrollPane	
     */
    private JScrollPane getJScrollPane1() {
        if (jScrollPane1 == null) {
            jScrollPane1 = new JScrollPane();
            jScrollPane1.setPreferredSize(new java.awt.Dimension(350,55));
            jScrollPane1.setViewportView(getJTable());
        }
        return jScrollPane1;
    }


    /**
     * This method initializes jTable	
     * 	
     * @return javax.swing.JTable	
     */
    private JTable getJTable() {
        if (jTable == null) {
            fvPropertyTableModel = new DefaultTableModel();
            jTable = new JTable(fvPropertyTableModel);
            fvPropertyTableModel.addColumn("Name");
            fvPropertyTableModel.addColumn("Value");
            jTable.setRowHeight(20);
            jTable.setPreferredSize(new java.awt.Dimension(350,55));
        }
        return jTable;
    }


    /**
     * This method initializes jPanel4	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel4() {
        if (jPanel4 == null) {
            jPanel4 = new JPanel();
            jPanel4.setPreferredSize(new java.awt.Dimension(80,55));
            
            jPanel4.add(getJButton(), null);
            jPanel4.add(getJButton1(), null);
        }
        return jPanel4;
    }


    /**
     * This method initializes jButton1	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton1() {
        if (jButton1 == null) {
            jButton1 = new JButton();
            jButton1.setPreferredSize(new java.awt.Dimension(80,20));
            jButton1.setText("Delete");
        }
        return jButton1;
    }


    /**
     * This method initializes jPanelFvImageC	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanelFvImageC() {
        if (jPanelFvImageC == null) {
            jLabel4 = new JLabel();
            jLabel4.setPreferredSize(new Dimension(38, 20));
            jLabel4.setText("Name");
            jLabel3 = new JLabel();
            jLabel3.setText("FV Image Names");
            jLabel3.setPreferredSize(new java.awt.Dimension(150,20));
            jLabel2 = new JLabel();
            jLabel2.setText("Type");
            jLabel2.setPreferredSize(new java.awt.Dimension(60,20));
            FlowLayout flowLayout3 = new FlowLayout();
            flowLayout3.setAlignment(java.awt.FlowLayout.LEFT);
            flowLayout3.setHgap(5);
            jPanelFvImageC = new JPanel();
            jPanelFvImageC.setLayout(flowLayout3);
            jPanelFvImageC.add(getJPanel6(), null);
            
            jPanelFvImageC.add(getJPanel5(), null);
            jPanelFvImageC.add(getJScrollPane3(), null);
        }
        return jPanelFvImageC;
    }


    /**
     * This method initializes jCheckBox2	
     * 	
     * @return javax.swing.JCheckBox	
     */
    private JCheckBox getJCheckBox2() {
        if (jCheckBox2 == null) {
            jCheckBox2 = new JCheckBox();
            jCheckBox2.setText("FV Image Parameters");
            jCheckBox2.setPreferredSize(new java.awt.Dimension(200,20));
        }
        return jCheckBox2;
    }


    /**
     * This method initializes jComboBox	
     * 	
     * @return javax.swing.JComboBox	
     */
    private JComboBox getJComboBox() {
        if (jComboBox == null) {
            jComboBox = new JComboBox();
            jComboBox.addItem("ValidImageNames");
            jComboBox.addItem("Attributes");
            jComboBox.addItem("Options");
            jComboBox.addItem("Components");
            jComboBox.setPreferredSize(new java.awt.Dimension(170,20));
            jComboBox.addItemListener(new ItemListener() {

                public void itemStateChanged(ItemEvent arg0) {
                    // TODO disable attribute settings when ValidImageNames selected.
                    
                }
                
            });
        }
        return jComboBox;
    }


    /**
     * This method initializes jTextField2	
     * 	
     * @return javax.swing.JTextField	
     */
    private JTextField getJTextField2() {
        if (jTextField2 == null) {
            jTextField2 = new JTextField();
            jTextField2.setPreferredSize(new java.awt.Dimension(140,20));
        }
        return jTextField2;
    }


    /**
     * This method initializes jButton2	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton2() {
        if (jButton2 == null) {
            jButton2 = new JButton();
            jButton2.setPreferredSize(new Dimension(80, 20));
            jButton2.setText("Add");
        }
        return jButton2;
    }


    /**
     * This method initializes jScrollPane2	
     * 	
     * @return javax.swing.JScrollPane	
     */
    private JScrollPane getJScrollPane2() {
        if (jScrollPane2 == null) {
            jScrollPane2 = new JScrollPane();
            jScrollPane2.setPreferredSize(new java.awt.Dimension(350,50));
            jScrollPane2.setViewportView(getJTable1());
        }
        return jScrollPane2;
    }


    /**
     * This method initializes jTable1	
     * 	
     * @return javax.swing.JTable	
     */
    private JTable getJTable1() {
        if (jTable1 == null) {
            fvImageNameTableModel = new DefaultTableModel();
            jTable1 = new JTable(fvImageNameTableModel);
            fvImageNameTableModel.addColumn("FV Image Name");
            jTable1.setPreferredSize(new java.awt.Dimension(350,50));
        }
        return jTable1;
    }


    /**
     * This method initializes jButton3	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton3() {
        if (jButton3 == null) {
            jButton3 = new JButton();
            jButton3.setPreferredSize(new Dimension(80, 20));
            jButton3.setText("Delete");
        }
        return jButton3;
    }



  			
  private JPanel getJPanel5() {
       			

 if (jPanel5 == null) {
     //ToDo add ButtonGroup for RadioButtons
            jPanel5 = new JPanel();
            jPanel5.setPreferredSize(new java.awt.Dimension(440,120));
            
            GridLayout gridLayout = new GridLayout();
            gridLayout.setRows(5);
            gridLayout.setColumns(3);
            jPanel5.setLayout(gridLayout);
            jPanel5.add(getJCheckBox3(), null);
            
            jLabel5 = new JLabel();
            jLabel5.setText("EFI_ERASE_POLARITY");
			jPanel5.add(jLabel5, null);
			jPanel5.add(getJComboBox1(), null);
			jPanel5.add(getJCheckBox4(), null);
			jPanel5.add(getJCheckBox5(), null);
			jPanel5.add(getJCheckBox6(), null);
			jPanel5.add(getJCheckBox7(), null);
			jPanel5.add(getJCheckBox8(), null);
			jPanel5.add(getJCheckBox9(), null);
			jPanel5.add(getJCheckBox10(), null);
			jPanel5.add(getJCheckBox11(), null);
			jPanel5.add(getJCheckBox12(), null);
			jPanel5.add(getJCheckBox13(), null);
            jPanel5.add(new JLabel(), null);
            jPanel5.add(getJButton4(), null);
			jPanel5.setBorder(javax.swing.BorderFactory.createEtchedBorder(javax.swing.border.EtchedBorder.LOWERED));
            
            
            			

            
        }
        return jPanel5;
    }


    /**
     * This method initializes jButton4	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton4() {
        if (jButton4 == null) {
            jButton4 = new JButton();
            jButton4.setPreferredSize(new java.awt.Dimension(80,20));
            jButton4.setText("More FV");
        }
        return jButton4;
    }


    /**
     * This method initializes jScrollPane3	
     * 	
     * @return javax.swing.JScrollPane	
     */
    private JScrollPane getJScrollPane3() {
        if (jScrollPane3 == null) {
            jScrollPane3 = new JScrollPane();
            jScrollPane3.setPreferredSize(new java.awt.Dimension(440,150));
            jScrollPane3.setViewportView(getJTable2());
        }
        return jScrollPane3;
    }


    /**
     * This method initializes jTable2	
     * 	
     * @return javax.swing.JTable	
     */
    private JTable getJTable2() {
        if (jTable2 == null) {
            fvImageParaTableModel = new DefaultTableModel();
            jTable2 = new JTable(fvImageParaTableModel);
            fvImageParaTableModel.addColumn("FvImageNames");
            fvImageParaTableModel.addColumn("ReadStatus");
            fvImageParaTableModel.addColumn("WriteStatus");
            fvImageParaTableModel.addColumn("Lock Status");
            fvImageParaTableModel.addColumn("MM");
        }
        return jTable2;
    }


    /**
     * This method initializes jButton6	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton6() {
        if (jButton6 == null) {
            jButton6 = new JButton();
            jButton6.setPreferredSize(new java.awt.Dimension(150,20));
            jButton6.setText("Delete Row");
        }
        return jButton6;
    }


    /**
     * This method initializes jCheckBox3	
     * 	
     * @return javax.swing.JCheckBox	
     */
    private JCheckBox getJCheckBox3() {
        if (jCheckBox3 == null) {
            jCheckBox3 = new JCheckBox();
            jCheckBox3.setText("FV Image Options");
        }
        return jCheckBox3;
    }


    /**
     * This method initializes jComboBox1	
     * 	
     * @return javax.swing.JComboBox	
     */
    private JComboBox getJComboBox1() {
        if (jComboBox1 == null) {
            jComboBox1 = new JComboBox();
            jComboBox1.setPreferredSize(new java.awt.Dimension(20,20));
            jComboBox1.addItem("1");
            jComboBox1.addItem("0");
            jComboBox1.setSelectedIndex(0);
        }
        return jComboBox1;
    }


    /**
     * This method initializes jCheckBox4	
     * 	
     * @return javax.swing.JCheckBox	
     */
    private JCheckBox getJCheckBox4() {
        if (jCheckBox4 == null) {
            jCheckBox4 = new JCheckBox();
            jCheckBox4.setText("Read Disable CAP");
        }
        return jCheckBox4;
    }


    /**
     * This method initializes jCheckBox5	
     * 	
     * @return javax.swing.JCheckBox	
     */
    private JCheckBox getJCheckBox5() {
        if (jCheckBox5 == null) {
            jCheckBox5 = new JCheckBox();
            jCheckBox5.setText("Read Enable CAP");
        }
        return jCheckBox5;
    }


    /**
     * This method initializes jCheckBox6	
     * 	
     * @return javax.swing.JCheckBox	
     */
    private JCheckBox getJCheckBox6() {
        if (jCheckBox6 == null) {
            jCheckBox6 = new JCheckBox();
            jCheckBox6.setText("Read Status");
        }
        return jCheckBox6;
    }


    /**
     * This method initializes jCheckBox7	
     * 	
     * @return javax.swing.JCheckBox	
     */
    private JCheckBox getJCheckBox7() {
        if (jCheckBox7 == null) {
            jCheckBox7 = new JCheckBox();
            jCheckBox7.setText("Write Disable CAP");
        }
        return jCheckBox7;
    }


    /**
     * This method initializes jCheckBox8	
     * 	
     * @return javax.swing.JCheckBox	
     */
    private JCheckBox getJCheckBox8() {
        if (jCheckBox8 == null) {
            jCheckBox8 = new JCheckBox();
            jCheckBox8.setText("Write Enable CAP");
        }
        return jCheckBox8;
    }


    /**
     * This method initializes jCheckBox9	
     * 	
     * @return javax.swing.JCheckBox	
     */
    private JCheckBox getJCheckBox9() {
        if (jCheckBox9 == null) {
            jCheckBox9 = new JCheckBox();
            jCheckBox9.setText("Write Status");
        }
        return jCheckBox9;
    }


    /**
     * This method initializes jCheckBox10	
     * 	
     * @return javax.swing.JCheckBox	
     */
    private JCheckBox getJCheckBox10() {
        if (jCheckBox10 == null) {
            jCheckBox10 = new JCheckBox();
            jCheckBox10.setText("Lock CAP");
        }
        return jCheckBox10;
    }


    /**
     * This method initializes jCheckBox11	
     * 	
     * @return javax.swing.JCheckBox	
     */
    private JCheckBox getJCheckBox11() {
        if (jCheckBox11 == null) {
            jCheckBox11 = new JCheckBox();
            jCheckBox11.setText("Lock Status");
        }
        return jCheckBox11;
    }


    /**
     * This method initializes jCheckBox12	
     * 	
     * @return javax.swing.JCheckBox	
     */
    private JCheckBox getJCheckBox12() {
        if (jCheckBox12 == null) {
            jCheckBox12 = new JCheckBox();
            jCheckBox12.setText("Memory Mapped");
        }
        return jCheckBox12;
    }


    /**
     * This method initializes jCheckBox13	
     * 	
     * @return javax.swing.JCheckBox	
     */
    private JCheckBox getJCheckBox13() {
        if (jCheckBox13 == null) {
            jCheckBox13 = new JCheckBox();
            jCheckBox13.setText("Sticky Write");
        }
        return jCheckBox13;
    }


    /**
     * This method initializes jPanel6	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel6() {
        if (jPanel6 == null) {
            jPanel6 = new JPanel();
            jPanel6.setPreferredSize(new java.awt.Dimension(450,120));
            jPanel6.setBorder(javax.swing.BorderFactory.createEtchedBorder(javax.swing.border.EtchedBorder.RAISED));
            jPanel6.add(getJCheckBox2(), null);
            jPanel6.add(jLabel2, null);
            jPanel6.add(getJComboBox(), null);
            jPanel6.add(new StarLabel(), null);
            jPanel6.add(jLabel3, null);
            jPanel6.add(jLabel4, null);
            jPanel6.add(getJTextField2(), null);
            jPanel6.add(getJButton2(), null);
            jPanel6.add(getJScrollPane2(), null);
            jPanel6.add(getJButton3(), null);
        }
        return jPanel6;
    }


    /**
     * This method initializes jPanelFdfN	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanelFdfN() {
        if (jPanelFdfN == null) {
            GridLayout gridLayout1 = new GridLayout();
            gridLayout1.setRows(2);
            gridLayout1.setColumns(0);
            jPanelFdfN = new JPanel();
            jPanelFdfN.setLayout(gridLayout1);
            jPanelFdfN.setPreferredSize(new java.awt.Dimension(450,60));
            jPanelFdfN.add(getJPanel9(), null);
            jPanelFdfN.add(getJPanel8(), null);
        }
        return jPanelFdfN;
    }


    /**
     * This method initializes jPanelFdfW	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanelFdfW() {
        if (jPanelFdfW == null) {
            FlowLayout flowLayout9 = new FlowLayout();
            flowLayout9.setVgap(0);
            jPanelFdfW = new JPanel();
            jPanelFdfW.setLayout(flowLayout9);
            jPanelFdfW.setPreferredSize(new java.awt.Dimension(120,300));
            jPanelFdfW.add(getJCheckBox15(), null);
            jPanelFdfW.add(getJScrollPane6(), null);
        }
        return jPanelFdfW;
    }


    /**
     * This method initializes jPanelFdfS	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanelFdfS() {
        if (jPanelFdfS == null) {
            jPanelFdfS = new JPanel();
            jPanelFdfS.setLayout(new FlowLayout());
            jPanelFdfS.setPreferredSize(new java.awt.Dimension(500,250));
            jPanelFdfS.add(getJPanel10(), null);
            jPanelFdfS.add(getJPanel11(), null);
        }
        return jPanelFdfS;
    }


    /**
     * This method initializes jRadioButton	
     * 	
     * @return javax.swing.JRadioButton	
     */
    private JRadioButton getJRadioButton() {
        if (jRadioButton == null) {
            jRadioButton = new JRadioButton();
            jRadioButton.setText("Choose Existing FDF File");
        }
        return jRadioButton;
    }


    /**
     * This method initializes jTextField3	
     * 	
     * @return javax.swing.JTextField	
     */
    private JTextField getJTextField3() {
        if (jTextField3 == null) {
            jTextField3 = new JTextField();
            jTextField3.setPreferredSize(new java.awt.Dimension(200,20));
        }
        return jTextField3;
    }


    /**
     * This method initializes jPanel9	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel9() {
        if (jPanel9 == null) {
            FlowLayout flowLayout5 = new FlowLayout();
            flowLayout5.setAlignment(java.awt.FlowLayout.LEFT);
            jPanel9 = new JPanel();
            jPanel9.setLayout(flowLayout5);
            jPanel9.add(getJRadioButton(), null);
            jPanel9.add(getJTextField3(), null);
            jPanel9.add(getJButton5(), null);
        }
        return jPanel9;
    }


    /**
     * This method initializes jPanel8	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel8() {
        if (jPanel8 == null) {
            FlowLayout flowLayout6 = new FlowLayout();
            flowLayout6.setAlignment(java.awt.FlowLayout.LEFT);
            jPanel8 = new JPanel();
            jPanel8.setLayout(flowLayout6);
            jPanel8.add(getJRadioButton1(), null);
        }
        return jPanel8;
    }


    /**
     * This method initializes jButton5	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton5() {
        if (jButton5 == null) {
            jButton5 = new JButton();
            jButton5.setPreferredSize(new java.awt.Dimension(80,20));
            jButton5.setText("Browse");
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
            jRadioButton1.setText("Flash Data Definition");
        }
        return jRadioButton1;
    }


    /**
     * This method initializes jPanel10	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel10() {
        if (jPanel10 == null) {
            jLabel12 = new JLabel();
            jLabel12.setText("Type");
            jLabel11 = new JLabel();
            jLabel11.setText("Image Definition Name");
            jLabel10 = new JLabel();
            jLabel10.setText("Data");
            FlowLayout flowLayout7 = new FlowLayout();
            flowLayout7.setHgap(5);
            flowLayout7.setAlignment(java.awt.FlowLayout.LEFT);
            jLabel9 = new JLabel();
            jLabel9.setText("Optional");
            jLabel8 = new JLabel();
            jLabel8.setText("SubRegion");
            jLabel8.setPreferredSize(new java.awt.Dimension(71,16));
            jLabel7 = new JLabel();
            jLabel7.setText("Region");
            jLabel7.setPreferredSize(new java.awt.Dimension(59,16));
            jLabel6 = new JLabel();
            jLabel6.setText("Name");
            jLabel6.setPreferredSize(new java.awt.Dimension(53,20));
            jPanel10 = new JPanel();
            jPanel10.setPreferredSize(new java.awt.Dimension(480,120));
            jPanel10.setBorder(javax.swing.BorderFactory.createEtchedBorder(javax.swing.border.EtchedBorder.LOWERED));
            jPanel10.setLayout(flowLayout7);
            jPanel10.add(getJCheckBox14(), null);
            jPanel10.add(jLabel6, null);
            jPanel10.add(getJTextField4(), null);
            jPanel10.add(jLabel11, null);
            jPanel10.add(getJTextField7(), null);
            jPanel10.add(jLabel12, null);
            jPanel10.add(getJComboBox3(), null);
            jPanel10.add(jLabel9, null);
            jPanel10.add(getJComboBox2(), null);
            jPanel10.add(jLabel7, null);
            jPanel10.add(getJTextField5(), null);
            jPanel10.add(jLabel8, null);
            jPanel10.add(getJTextField6(), null);
            jPanel10.add(jLabel10, null);
            jPanel10.add(getJScrollPane5(), null);
            jPanel10.add(getJButton7(), null);
            jPanel10.add(getJButton8(), null);
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
            FlowLayout flowLayout8 = new FlowLayout();
            flowLayout8.setAlignment(java.awt.FlowLayout.LEFT);
            jPanel11 = new JPanel();
            jPanel11.setLayout(flowLayout8);
            jPanel11.add(getJScrollPane4(), null);
        }
        return jPanel11;
    }


    /**
     * This method initializes jScrollPane4	
     * 	
     * @return javax.swing.JScrollPane	
     */
    private JScrollPane getJScrollPane4() {
        if (jScrollPane4 == null) {
            jScrollPane4 = new JScrollPane();
            jScrollPane4.setPreferredSize(new java.awt.Dimension(480,100));
            jScrollPane4.setViewportView(getJTable3());
        }
        return jScrollPane4;
    }


    /**
     * This method initializes jTable3	
     * 	
     * @return javax.swing.JTable	
     */
    private JTable getJTable3() {
        if (jTable3 == null) {
            fdfImageDefTableModel = new DefaultTableModel();
            jTable3 = new JTable(fdfImageDefTableModel);
            fdfImageDefTableModel.addColumn("DeviceImageName");
            fdfImageDefTableModel.addColumn("Type");
            fdfImageDefTableModel.addColumn("DefName");
            fdfImageDefTableModel.addColumn("Region");
            fdfImageDefTableModel.addColumn("SubRegion");
            fdfImageDefTableModel.addColumn("Opt");
        }
        return jTable3;
    }


    /**
     * This method initializes jCheckBox14	
     * 	
     * @return javax.swing.JCheckBox	
     */
    private JCheckBox getJCheckBox14() {
        if (jCheckBox14 == null) {
            jCheckBox14 = new JCheckBox();
            jCheckBox14.setText("Flash Device Image");
        }
        return jCheckBox14;
    }


    /**
     * This method initializes jTextField4	
     * 	
     * @return javax.swing.JTextField	
     */
    private JTextField getJTextField4() {
        if (jTextField4 == null) {
            jTextField4 = new JTextField();
            jTextField4.setPreferredSize(new java.awt.Dimension(150,20));
        }
        return jTextField4;
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
     * This method initializes jComboBox2	
     * 	
     * @return javax.swing.JComboBox	
     */
    private JComboBox getJComboBox2() {
        if (jComboBox2 == null) {
            jComboBox2 = new JComboBox();
            jComboBox2.addItem("True");
            jComboBox2.addItem("False");
            jComboBox2.setPreferredSize(new java.awt.Dimension(70,20));
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
            jComboBox3.addItem("RawData");
            jComboBox3.addItem("File");
            jComboBox3.setSelectedIndex(0);
        }
        return jComboBox3;
    }


    /**
     * This method initializes jButton7	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton7() {
        if (jButton7 == null) {
            jButton7 = new JButton();
            jButton7.setPreferredSize(new java.awt.Dimension(80,20));
            jButton7.setText("Add");
        }
        return jButton7;
    }


    /**
     * This method initializes jButton8	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton8() {
        if (jButton8 == null) {
            jButton8 = new JButton();
            jButton8.setPreferredSize(new java.awt.Dimension(80,20));
            jButton8.setText("Remove");
        }
        return jButton8;
    }


    /**
     * This method initializes jScrollPane5	
     * 	
     * @return javax.swing.JScrollPane	
     */
    private JScrollPane getJScrollPane5() {
        if (jScrollPane5 == null) {
            jScrollPane5 = new JScrollPane();
            jScrollPane5.setPreferredSize(new java.awt.Dimension(250,30));
            jScrollPane5.setViewportView(getJTextArea());
        }
        return jScrollPane5;
    }


    /**
     * This method initializes jTextArea	
     * 	
     * @return javax.swing.JTextArea	
     */
    private JTextArea getJTextArea() {
        if (jTextArea == null) {
            jTextArea = new JTextArea();
        }
        return jTextArea;
    }


    /**
     * This method initializes jTextField7	
     * 	
     * @return javax.swing.JTextField	
     */
    private JTextField getJTextField7() {
        if (jTextField7 == null) {
            jTextField7 = new JTextField();
            jTextField7.setPreferredSize(new java.awt.Dimension(80,20));
        }
        return jTextField7;
    }


    /**
     * This method initializes jCheckBox15	
     * 	
     * @return javax.swing.JCheckBox	
     */
    private JCheckBox getJCheckBox15() {
        if (jCheckBox15 == null) {
            jCheckBox15 = new JCheckBox();
            jCheckBox15.setText("Flash Device Info");
            jCheckBox15.setPreferredSize(new java.awt.Dimension(120,20));
        }
        return jCheckBox15;
    }


    /**
     * This method initializes jScrollPane6	
     * 	
     * @return javax.swing.JScrollPane	
     */
    private JScrollPane getJScrollPane6() {
        if (jScrollPane6 == null) {
            jScrollPane6 = new JScrollPane();
            jScrollPane6.setPreferredSize(new java.awt.Dimension(120,220));
            jScrollPane6.setViewportView(getJList());
        }
        return jScrollPane6;
    }


    /**
     * This method initializes jList	
     * 	
     * @return javax.swing.JList	
     */
    private JList getJList() {
        if (jList == null) {
            jList = new JList();
            jList.setSize(new java.awt.Dimension(117,220));
        }
        return jList;
    }


    /**
     * This method initializes jTabbedPane1	
     * 	
     * @return javax.swing.JTabbedPane	
     */
    private JTabbedPane getJTabbedPane1() {
        if (jTabbedPane1 == null) {
            jTabbedPane1 = new JTabbedPane();
            jTabbedPane1.setTabPlacement(javax.swing.JTabbedPane.TOP);
            jTabbedPane1.addTab("Basics", null, getJPanelDevInfoBase(), null);
            jTabbedPane1.addTab("Blocks", null, getJPanelDevInfoBlocks(), null);
            jTabbedPane1.addTab("Regions", null, getJPanelDevInfoRegions(), null);
            
        }
        return jTabbedPane1;
    }


    /**
     * This method initializes jPanelDevInfoBase	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanelDevInfoBase() {
        if (jPanelDevInfoBase == null) {
            jLabel16 = new JLabel();
            jLabel16.setText("Erase Polarity");
            jLabel15 = new JLabel();
            jLabel15.setText("Base Address");
            FlowLayout flowLayout10 = new FlowLayout();
            flowLayout10.setAlignment(java.awt.FlowLayout.CENTER);
            jLabel14 = new JLabel();
            jLabel14.setText("Device Size");
            jLabel13 = new JLabel();
            jLabel13.setText("Device Info Name");
            jPanelDevInfoBase = new JPanel();
            jPanelDevInfoBase.setLayout(flowLayout10);
            jPanelDevInfoBase.add(jLabel13, null);
            jPanelDevInfoBase.add(getJTextField8(), null);
            jPanelDevInfoBase.add(jLabel14, null);
            jPanelDevInfoBase.add(getJTextField9(), null);
            jPanelDevInfoBase.add(jLabel15, null);
            jPanelDevInfoBase.add(getJTextField10(), null);
            jPanelDevInfoBase.add(jLabel16, null);
            jPanelDevInfoBase.add(getJComboBox4(), null);
            jPanelDevInfoBase.add(getJButton9(), null);
            jPanelDevInfoBase.add(getJButton10(), null);
        }
        return jPanelDevInfoBase;
    }


    /**
     * This method initializes jPanelDevInfoBlocks	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanelDevInfoBlocks() {
        if (jPanelDevInfoBlocks == null) {
            jPanelDevInfoBlocks = new JPanel();
            jPanelDevInfoBlocks.setLayout(new BorderLayout());
            jPanelDevInfoBlocks.add(getJScrollPane7(), java.awt.BorderLayout.NORTH);
//            jPanelDevInfoBlocks.add(getJButton11(), java.awt.BorderLayout.SOUTH);
            jPanelDevInfoBlocks.add(getJPanel13(), java.awt.BorderLayout.SOUTH);
//            jPanelDevInfoBlocks.add(getJButton12(), java.awt.BorderLayout.SOUTH);
        }
        return jPanelDevInfoBlocks;
    }


    /**
     * This method initializes jPanelDevInfoRegions	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanelDevInfoRegions() {
        if (jPanelDevInfoRegions == null) {
            jLabel18 = new JLabel();
            jLabel18.setText("SubRegion Settings");
            jLabel17 = new JLabel();
            jLabel17.setText("Region Settings");
            jPanelDevInfoRegions = new JPanel();
            jPanelDevInfoRegions.setLayout(new BorderLayout());
//            jPanelDevInfoRegions.add(jLabel17, java.awt.BorderLayout.NORTH);
            jPanelDevInfoRegions.add(getJPanel7(), java.awt.BorderLayout.NORTH);
            jPanelDevInfoRegions.add(getJPanel12(), java.awt.BorderLayout.SOUTH);
//            jPanelDevInfoRegions.add(getJScrollPane8(), java.awt.BorderLayout.EAST);
            jPanelDevInfoRegions.add(getJPanel14(), java.awt.BorderLayout.CENTER);
//            jPanelDevInfoRegions.add(jLabel18, java.awt.BorderLayout.WEST);
//            jPanelDevInfoRegions.add(getJButton13(), java.awt.BorderLayout.CENTER);
//            jPanelDevInfoRegions.add(getJButton14(), java.awt.BorderLayout.SOUTH);
        }
        return jPanelDevInfoRegions;
    }


    /**
     * This method initializes jTextField8	
     * 	
     * @return javax.swing.JTextField	
     */
    private JTextField getJTextField8() {
        if (jTextField8 == null) {
            jTextField8 = new JTextField();
            jTextField8.setPreferredSize(new java.awt.Dimension(80,20));
        }
        return jTextField8;
    }


    /**
     * This method initializes jTextField9	
     * 	
     * @return javax.swing.JTextField	
     */
    private JTextField getJTextField9() {
        if (jTextField9 == null) {
            jTextField9 = new JTextField();
            jTextField9.setPreferredSize(new java.awt.Dimension(80,20));
        }
        return jTextField9;
    }


    /**
     * This method initializes jTextField10	
     * 	
     * @return javax.swing.JTextField	
     */
    private JTextField getJTextField10() {
        if (jTextField10 == null) {
            jTextField10 = new JTextField();
            jTextField10.setPreferredSize(new java.awt.Dimension(80,20));
        }
        return jTextField10;
    }


    /**
     * This method initializes jComboBox4	
     * 	
     * @return javax.swing.JComboBox	
     */
    private JComboBox getJComboBox4() {
        if (jComboBox4 == null) {
            jComboBox4 = new JComboBox();
            jComboBox4.setPreferredSize(new java.awt.Dimension(40,20));
        }
        return jComboBox4;
    }


    /**
     * This method initializes jButton9	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton9() {
        if (jButton9 == null) {
            jButton9 = new JButton();
            jButton9.setPreferredSize(new java.awt.Dimension(80,20));
            jButton9.setText("Add");
        }
        return jButton9;
    }


    /**
     * This method initializes jButton10	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton10() {
        if (jButton10 == null) {
            jButton10 = new JButton();
            jButton10.setPreferredSize(new java.awt.Dimension(80,20));
            jButton10.setText("Remove");
        }
        return jButton10;
    }


    /**
     * This method initializes jScrollPane7	
     * 	
     * @return javax.swing.JScrollPane	
     */
    private JScrollPane getJScrollPane7() {
        if (jScrollPane7 == null) {
            jScrollPane7 = new JScrollPane();
            jScrollPane7.setPreferredSize(new java.awt.Dimension(353,175));
            jScrollPane7.setViewportView(getJTable4());
        }
        return jScrollPane7;
    }


    /**
     * This method initializes jTable4	
     * 	
     * @return javax.swing.JTable	
     */
    private JTable getJTable4() {
        if (jTable4 == null) {
            fdfBlocksTableModel = new DefaultTableModel();
            jTable4 = new JTable(fdfBlocksTableModel);
            fdfBlocksTableModel.addColumn("Name");
            fdfBlocksTableModel.addColumn("Size");
            fdfBlocksTableModel.addColumn("Flags");
        }
        return jTable4;
    }


    /**
     * This method initializes jButton11	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton11() {
        if (jButton11 == null) {
            jButton11 = new JButton();
            jButton11.setText("Insert");
            jButton11.setPreferredSize(new java.awt.Dimension(67,20));
        }
        return jButton11;
    }


    /**
     * This method initializes jButton12	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton12() {
        if (jButton12 == null) {
            jButton12 = new JButton();
            jButton12.setText("Delete");
            jButton12.setPreferredSize(new java.awt.Dimension(70,20));
        }
        return jButton12;
    }


    /**
     * This method initializes jScrollPane8	
     * 	
     * @return javax.swing.JScrollPane	
     */
    private JScrollPane getJScrollPane8() {
        if (jScrollPane8 == null) {
            jScrollPane8 = new JScrollPane();
            jScrollPane8.setPreferredSize(new java.awt.Dimension(450,60));
            jScrollPane8.setViewportView(getJTable5());
        }
        return jScrollPane8;
    }


    /**
     * This method initializes jTable5	
     * 	
     * @return javax.swing.JTable	
     */
    private JTable getJTable5() {
        if (jTable5 == null) {
            fdfRegionsTableModel = new DefaultTableModel();
            jTable5 = new JTable(fdfRegionsTableModel);
            fdfRegionsTableModel.addColumn("Name");
            fdfRegionsTableModel.addColumn("Size");
            fdfRegionsTableModel.addColumn("Attributes");
            fdfRegionsTableModel.addColumn("AreaType");
            
        }
        return jTable5;
    }


    /**
     * This method initializes jScrollPane9	
     * 	
     * @return javax.swing.JScrollPane	
     */
    private JScrollPane getJScrollPane9() {
        if (jScrollPane9 == null) {
            jScrollPane9 = new JScrollPane();
            jScrollPane9.setPreferredSize(new java.awt.Dimension(450,100));
            jScrollPane9.setViewportView(getJTable6());
        }
        return jScrollPane9;
    }


    /**
     * This method initializes jTable6	
     * 	
     * @return javax.swing.JTable	
     */
    private JTable getJTable6() {
        if (jTable6 == null) {
            fdfSubRegionsTableModel = new DefaultTableModel();
            jTable6 = new JTable(fdfSubRegionsTableModel);
            fdfSubRegionsTableModel.addColumn("Name");
            fdfSubRegionsTableModel.addColumn("CreateHOB");
            fdfSubRegionsTableModel.addColumn("Size");
            fdfSubRegionsTableModel.addColumn("Attributes");
            fdfSubRegionsTableModel.addColumn("AreaType");
            fdfSubRegionsTableModel.addColumn("NameGuid");
        }
        return jTable6;
    }


    /**
     * This method initializes jButton13	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton13() {
        if (jButton13 == null) {
            jButton13 = new JButton();
            jButton13.setText("Insert");
            jButton13.setPreferredSize(new java.awt.Dimension(67,20));
        }
        return jButton13;
    }


    /**
     * This method initializes jButton14	
     * 	
     * @return javax.swing.JButton	
     */
    private JButton getJButton14() {
        if (jButton14 == null) {
            jButton14 = new JButton();
            jButton14.setText("Delete");
            jButton14.setPreferredSize(new java.awt.Dimension(70,20));
        }
        return jButton14;
    }


    /**
     * This method initializes jPanel7	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel7() {
        if (jPanel7 == null) {
            FlowLayout flowLayout13 = new FlowLayout();
            flowLayout13.setAlignment(java.awt.FlowLayout.LEFT);
            flowLayout13.setVgap(0);
            flowLayout13.setHgap(0);
            jPanel7 = new JPanel();
            jPanel7.setLayout(flowLayout13);
            jPanel7.setPreferredSize(new java.awt.Dimension(10,70));
//            jPanel7.add(jLabel17, null);
            jPanel7.add(getJScrollPane8(), null);
        }
        return jPanel7;
    }


    /**
     * This method initializes jPanel12	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel12() {
        if (jPanel12 == null) {
            FlowLayout flowLayout14 = new FlowLayout();
            flowLayout14.setAlignment(java.awt.FlowLayout.LEFT);
            flowLayout14.setVgap(0);
            flowLayout14.setHgap(0);
            jPanel12 = new JPanel();
            jPanel12.setLayout(flowLayout14);
            jPanel12.setPreferredSize(new java.awt.Dimension(10,110));

            jPanel12.add(getJScrollPane9(), null);
        }
        return jPanel12;
    }


    /**
     * This method initializes jPanel13	
     * 	
     * @return javax.swing.JPanel	
     */
    private JPanel getJPanel13() {
        if (jPanel13 == null) {
            FlowLayout flowLayout11 = new FlowLayout();
            flowLayout11.setAlignment(java.awt.FlowLayout.RIGHT);
            jPanel13 = new JPanel();
            jPanel13.setLayout(flowLayout11);
            jPanel13.setPreferredSize(new java.awt.Dimension(10,40));
            jPanel13.add(getJButton11(), null);
            jPanel13.add(getJButton12(), null);
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
            FlowLayout flowLayout12 = new FlowLayout();
            flowLayout12.setVgap(15);
            jPanel14 = new JPanel();
            jPanel14.setLayout(flowLayout12);
            jPanel14.add(jLabel18, null);
            jPanel14.add(getJButton13(), null);
            jPanel14.add(getJButton14(), null);
        }
        return jPanel14;
    }


    /**
     * @param args
     */
    public static void main(String[] args) {
        // TODO Auto-generated method stub
        new FpdFlash().setVisible(true);
    }

    /**
     * This method initializes this
     * 
     * @return void
     */
    private void initialize() {
        this.setSize(518, 650);
        this.setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
        this.setContentPane(getJContentPane());
        this.setTitle("JFrame");
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
            jContentPane.add(getJPanel(), java.awt.BorderLayout.EAST);
            jContentPane.add(getJPanel1(), java.awt.BorderLayout.SOUTH);
            jContentPane.add(getJPanel2(), java.awt.BorderLayout.WEST);
            jContentPane.add(getJPanel3(), java.awt.BorderLayout.NORTH);
            jContentPane.add(getJTabbedPane(), java.awt.BorderLayout.CENTER);
        }
        return jContentPane;
    }

}  //  @jve:decl-index=0:visual-constraint="10,10"
