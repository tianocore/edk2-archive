/** @file
  Java class SpdGuidDecls is GUI for create library definition elements of spd file.
 
Copyright (c) 2006, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/
package org.tianocore.frameworkwizard.packaging.ui;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.util.Vector;

import javax.swing.DefaultListModel;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JScrollPane;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.table.DefaultTableModel;

import org.tianocore.frameworkwizard.common.Tools;
import org.tianocore.frameworkwizard.common.ui.IInternalFrame;
import org.tianocore.frameworkwizard.common.ui.StarLabel;

/**
 GUI for create library definition elements of spd file.
  
 @since PackageEditor 1.0
**/
public class SpdGuidDecls extends IInternalFrame{
    static JFrame frame;
    
    

    private JTable jTable = null;

    private GuidDeclTableModel model = null;

    private JPanel jContentPane = null;

    private JTextField jTextFieldAdd = null;

    private JScrollPane jScrollPane = null;

    private JButton jButtonAdd = null;

    private JButton jButtonRemove = null;

    private JButton jButtonClearAll = null;

    private JButton jButtonCancel = null;

    private JButton jButtonOk = null;

    private JButton jButtonGen = null;
    
    private StarLabel jStarLabel1 = null;
    
    private StarLabel jStarLabel2 = null;
    
    private StarLabel jStarLabel3 = null;
    
    private StarLabel jStarLabel4 = null;



    private JLabel jLabelName = null;



    private JTextField jTextFieldName = null;



    private JLabel jLabelGuid = null;



    private JLabel jLabelVer = null;

    private GenGuidDialog guidDialog = null;

    private JTextField jTextFieldVersion = null;



    /**
      This method initializes this
     
     **/
    protected void initialize() {
        
        this.setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);

    }

    /**
      This method initializes jTextFieldAdd	
      	
      @return javax.swing.JTextField	
     **/
    protected JTextField getJTextFieldAdd() {
        if (jTextFieldAdd == null) {
            jTextFieldAdd = new JTextField();
            jTextFieldAdd.setBounds(new java.awt.Rectangle(137,35,337,20));
            
        }
        return jTextFieldAdd;
    }

    /**
      This method initializes jScrollPane	
      	
      @return javax.swing.JScrollPane	
     **/
    protected JScrollPane getJScrollPane() {
        if (jScrollPane == null) {
            jScrollPane = new JScrollPane();
            jScrollPane.setBounds(new java.awt.Rectangle(13,123,460,172));
            jScrollPane.setViewportView(getJTable());
        }
        return jScrollPane;
    }

    /**
    This method initializes jTable  
        
    @return javax.swing.JTable  
    **/
   protected JTable getJTable() {
       if (jTable == null) {
           model = new GuidDeclTableModel();
           jTable = new JTable(model);
           jTable.setRowHeight(20);
           model.addColumn("Name");
           model.addColumn("C_Name");
           model.addColumn("GUID Value");
           

       }
       return jTable;
   }
    /**
      This method initializes jButtonAdd	
      	
      @return javax.swing.JButton	
     **/
    protected JButton getJButtonAdd() {
        if (jButtonAdd == null) {
            jButtonAdd = new JButton();
            jButtonAdd.setBounds(new java.awt.Rectangle(167,95,90,20));
            jButtonAdd.setText("Add");
            jButtonAdd.addActionListener(this);
        }
        return jButtonAdd;
    }

    /**
      This method initializes jButtonRemove	
      	
      @return javax.swing.JButton	
     **/
    protected JButton getJButtonRemove() {
        if (jButtonRemove == null) {
            jButtonRemove = new JButton();
            jButtonRemove.setBounds(new java.awt.Rectangle(270,95,90,20));
            jButtonRemove.setText("Remove");
            jButtonRemove.addActionListener(this);
        }
        return jButtonRemove;
    }

    /**
      This method initializes jButtonRemoveAll	
      	
      @return javax.swing.JButton	
     **/
    protected JButton getJButtonClearAll() {
        if (jButtonClearAll == null) {
            jButtonClearAll = new JButton();
            jButtonClearAll.setBounds(new java.awt.Rectangle(380,95,90,20));
            jButtonClearAll.setText("Clear All");
            jButtonClearAll.addActionListener(this);
        }
        return jButtonClearAll;
    }

    /**
      This method initializes jButtonCancel	
      	
      @return javax.swing.JButton	
     **/
    protected JButton getJButtonCancel() {
        if (jButtonCancel == null) {
            jButtonCancel = new JButton();
            jButtonCancel.setPreferredSize(new java.awt.Dimension(90, 20));
            jButtonCancel.setLocation(new java.awt.Point(390, 305));
            jButtonCancel.setText("Cancel");
            jButtonCancel.setSize(new java.awt.Dimension(90, 20));
            jButtonCancel.addActionListener(this);
        }
        return jButtonCancel;
    }

    /**
      This method initializes jButton	
      	
      @return javax.swing.JButton	
     **/
    protected JButton getJButtonOk() {
        if (jButtonOk == null) {
            jButtonOk = new JButton();
            jButtonOk.setSize(new java.awt.Dimension(90, 20));
            jButtonOk.setText("OK");
            jButtonOk.setLocation(new java.awt.Point(290, 305));
            jButtonOk.addActionListener(this);
        }
        return jButtonOk;
    }

    /**
      This is the default constructor
     **/
    public SpdGuidDecls() {
        super();
        initialize();
        init();
        
    }

    /**
      This method initializes this
      
      @return void
     **/
    protected void init() {
        this.setContentPane(getJContentPane());
        
        this.setBounds(new java.awt.Rectangle(0, 0, 500, 370));
       
        initFrame();
    }

    /**
      This method initializes jContentPane
      
      @return javax.swing.JPanel
     **/
    protected JPanel getJContentPane() {
        if (jContentPane == null) {
            jLabelVer = new JLabel();
            jLabelVer.setBounds(new java.awt.Rectangle(14,60,111,20));
            jLabelVer.setText("C_Name");
            jLabelGuid = new JLabel();
            jLabelGuid.setBounds(new java.awt.Rectangle(15,35,112,20));
            jLabelGuid.setText("Guid Value");
            jLabelName = new JLabel();
            jLabelName.setBounds(new java.awt.Rectangle(15,10,113,20));
            jLabelName.setText("Name");
            jStarLabel1 = new StarLabel();
            jStarLabel1.setLocation(new java.awt.Point(0, 10));
            jStarLabel3 = new StarLabel();
            jStarLabel3.setLocation(new java.awt.Point(0, 35));
            jStarLabel4 = new StarLabel();
            jStarLabel4.setLocation(new java.awt.Point(0, 60));
            jStarLabel2 = new StarLabel();
            jStarLabel2.setLocation(new java.awt.Point(0, 85));
            jStarLabel2.setVisible(false);
            jContentPane = new JPanel();
            jContentPane.setLayout(null);
            jContentPane.add(jStarLabel1, null);
            jContentPane.add(jStarLabel2, null);
            jContentPane.add(jStarLabel3, null);
            jContentPane.add(jStarLabel4, null);
            jContentPane.add(jLabelVer, null);
            jContentPane.add(getJTextFieldVersion(), null);
            jContentPane.add(getJTextFieldAdd(), null);
            jContentPane.add(getJScrollPane(), null);
            jContentPane.add(getJButtonAdd(), null);
            jContentPane.add(getJButtonRemove(), null);
            jContentPane.add(getJButtonClearAll(), null);
            jContentPane.add(getJButtonCancel(), null);
            jContentPane.add(getJButtonOk(), null);
            
            jContentPane.add(getJButtonGen(), null);
            jContentPane.add(jLabelName, null);
            jContentPane.add(getJTextFieldName(), null);
            jContentPane.add(jLabelGuid, null);
        }
        return jContentPane;
    }

    /**
     fill ComboBoxes with pre-defined contents
    **/
    protected void initFrame() {
        
        this.setTitle("GUID Declarations");

    }

    /* (non-Javadoc)
     * @see java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
     */
    public void actionPerformed(ActionEvent arg0) {
        if (arg0.getSource() == jButtonOk) {
            this.save();
            this.dispose();

        }
        if (arg0.getSource() == jButtonCancel) {
            this.dispose();
        }

        if (arg0.getSource() == jButtonAdd) {
            String strVer = jTextFieldVersion.getText();
            
            //ToDo: check before add
            String[] row = {"", "", ""};
            row[2] = jTextFieldAdd.getText();
            row[1] = jTextFieldVersion.getText();
            row[0] = jTextFieldName.getText();
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
            guidDialog.setGuid(jTextFieldAdd.getText());
        }
        
        if (arg0.getActionCommand().equals("GenGuidValue")) {
            jTextFieldAdd.setText(guidDialog.getGuid());
        }
        
    }

    /**
     Add contents in list to sfc
    **/
    protected void save() {
        
    }

    /**
      This method initializes jButtonBrowse	
      	
      @return javax.swing.JButton	
     **/
    protected JButton getJButtonGen() {
        if (jButtonGen == null) {
            jButtonGen = new JButton();
            jButtonGen.setBounds(new java.awt.Rectangle(379,58,92,21));
            jButtonGen.setText("Gen GUID");
            jButtonGen.setPreferredSize(new java.awt.Dimension(34,20));
            jButtonGen.addActionListener(this);
        }
        return jButtonGen;
    }
    
    class GuidDeclTableModel extends DefaultTableModel {
        public boolean isCellEditable(){
            return false;
        }
    }
    
    /**
     * This method initializes jTextFieldName	
     * 	
     * @return javax.swing.JTextField	
     */
    protected JTextField getJTextFieldName() {
        if (jTextFieldName == null) {
            jTextFieldName = new JTextField();
            jTextFieldName.setBounds(new java.awt.Rectangle(138,10,337,20));
        }
        return jTextFieldName;
    }

    /**
     * This method initializes jTextFieldVersion	
     * 	
     * @return javax.swing.JTextField	
     */
    protected JTextField getJTextFieldVersion() {
        if (jTextFieldVersion == null) {
            jTextFieldVersion = new JTextField();
            jTextFieldVersion.setBounds(new java.awt.Rectangle(137,60,225,20));
        }
        return jTextFieldVersion;
    }

    public static void main(String[] args){
        new SpdGuidDecls().setVisible(true);
    }
}


