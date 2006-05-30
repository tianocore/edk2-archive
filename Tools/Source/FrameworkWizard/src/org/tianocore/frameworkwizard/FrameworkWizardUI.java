/** @file
 
 The main GUI for module editor. 
 
 Copyright (c) 2006, Intel Corporation
 All rights reserved. This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution.  The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php
 
 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 
 **/

package org.tianocore.frameworkwizard;

import java.awt.event.ActionEvent;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.WindowEvent;
import java.io.File;
import java.io.IOException;
import java.util.Vector;

import javax.swing.JButton;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JDesktopPane;
import javax.swing.JFileChooser;
import javax.swing.JInternalFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JSplitPane;
import javax.swing.JTabbedPane;
import javax.swing.border.BevelBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;

import org.apache.xmlbeans.XmlCursor;
import org.apache.xmlbeans.XmlException;
import org.apache.xmlbeans.XmlObject;
import org.apache.xmlbeans.XmlOptions;
import org.tianocore.FrameworkPlatformDescriptionDocument;
import org.tianocore.ModuleSurfaceAreaDocument;
import org.tianocore.PackageSurfaceAreaDocument;
import org.tianocore.frameworkwizard.common.DataType;
import org.tianocore.frameworkwizard.common.IFileFilter;
import org.tianocore.frameworkwizard.common.Identification;
import org.tianocore.frameworkwizard.common.Log;
import org.tianocore.frameworkwizard.common.OpeningFileList;
import org.tianocore.frameworkwizard.common.Tools;
import org.tianocore.frameworkwizard.common.ui.IDefaultMutableTreeNode;
import org.tianocore.frameworkwizard.common.ui.IDesktopManager;
import org.tianocore.frameworkwizard.common.ui.IFrame;
import org.tianocore.frameworkwizard.common.ui.ITree;
import org.tianocore.frameworkwizard.module.ModuleIdentification;
import org.tianocore.frameworkwizard.module.ui.ModuleBootModes;
import org.tianocore.frameworkwizard.module.ui.ModuleDataHubs;
import org.tianocore.frameworkwizard.module.ui.ModuleEvents;
import org.tianocore.frameworkwizard.module.ui.ModuleExterns;
import org.tianocore.frameworkwizard.module.ui.ModuleFormsets;
import org.tianocore.frameworkwizard.module.ui.ModuleGuids;
import org.tianocore.frameworkwizard.module.ui.ModuleHobs;
import org.tianocore.frameworkwizard.module.ui.ModuleLibraryClassDefinitions;
import org.tianocore.frameworkwizard.module.ui.ModulePCDs;
import org.tianocore.frameworkwizard.module.ui.ModulePackageDependencies;
import org.tianocore.frameworkwizard.module.ui.ModulePpis;
import org.tianocore.frameworkwizard.module.ui.ModuleProtocols;
import org.tianocore.frameworkwizard.module.ui.ModuleSourceFiles;
import org.tianocore.frameworkwizard.module.ui.ModuleSystemTables;
import org.tianocore.frameworkwizard.module.ui.ModuleVariables;
import org.tianocore.frameworkwizard.module.ui.MsaHeader;
import org.tianocore.frameworkwizard.packaging.PackageIdentification;
import org.tianocore.frameworkwizard.packaging.ui.SpdGuidDecls;
import org.tianocore.frameworkwizard.packaging.ui.SpdHeader;
import org.tianocore.frameworkwizard.packaging.ui.SpdLibClassDecls;
import org.tianocore.frameworkwizard.packaging.ui.SpdMsaFiles;
import org.tianocore.frameworkwizard.packaging.ui.SpdPackageHeaders;
import org.tianocore.frameworkwizard.packaging.ui.SpdPcdDefs;
import org.tianocore.frameworkwizard.packaging.ui.SpdPpiDecls;
import org.tianocore.frameworkwizard.packaging.ui.SpdProtocolDecls;
import org.tianocore.frameworkwizard.platform.PlatformIdentification;
import org.tianocore.frameworkwizard.workspace.Workspace;

/**
 The class is used to show main GUI of ModuleEditor
 It extends IFrame implements MouseListener, TreeSelectionListener
 
 @since ModuleEditor 1.0

 **/
public class FrameworkWizardUI extends IFrame implements MouseListener, TreeSelectionListener, ComponentListener,
                                             ChangeListener {
    ///
    /// Define class Serial Version UID
    ///
    private static final long serialVersionUID = -7103240960573031772L;

    //
    //Define class members
    //
    private String currentModule = "";

    private String saveFileName = "";

    private boolean isSaved = true;

    //
    // To save information of all files
    //
    private Vector<ModuleIdentification> vModuleList = new Vector<ModuleIdentification>();

    private Vector<PackageIdentification> vPackageList = new Vector<PackageIdentification>();

    private Vector<PlatformIdentification> vPlatformList = new Vector<PlatformIdentification>();

    private OpeningFileList lstOpeningFile = new OpeningFileList();

    private ModuleSurfaceAreaDocument xmlMsaDoc = null;

    private PackageSurfaceAreaDocument xmlSpdDoc = null;

    private FrameworkPlatformDescriptionDocument xmlFpdDoc = null;

    ///
    ///  0 - reserved; 1 - msa; 2 - mbd; 3 - lmsa; 4 - lmbd;
    ///
    private int currentModuleType = 0;

    private String windowTitle = "FrameworkWizard 1.0 ";

    private IDefaultMutableTreeNode dmtnRoot = null;

    private IDefaultMutableTreeNode dmtnModuleDescription = null;

    private IDefaultMutableTreeNode dmtnPackageDescription = null;

    private IDefaultMutableTreeNode dmtnPlatformDescription = null;

    private JPanel jContentPane = null;

    private JMenuBar jMenuBar = null;

    private JMenu jMenuFile = null;

    private JMenuItem jMenuItemFileNew = null;

    private JMenuItem jMenuItemFileSaveAs = null;

    private JMenuItem jMenuItemFileExit = null;

    private JMenu jMenuEdit = null;

    private JDesktopPane jDesktopPaneModule = null;

    private JDesktopPane jDesktopPanePackage = null;

    private JDesktopPane jDesktopPanePlatform = null;

    private JTabbedPane jTabbedPaneTree = null;

    private JTabbedPane jTabbedPaneEditor = null;

    private IDesktopManager iDesktopManager = new IDesktopManager();

    private JScrollPane jScrollPaneTree = null;

    private ITree iTree = null;

    private JMenu jMenuHelp = null;

    private JMenuItem jMenuItemHelpAbout = null;

    private JMenuItem jMenuItemEditDelete = null;

    private JPopupMenu jPopupMenu = null;

    private JMenuItem jMenuItemPopupAdd = null;

    private JMenuItem jMenuItemPopupUpdate = null;

    private JMenuItem jMenuItemPopupDelete = null;

    private Workspace ws = new Workspace();

    private static final int OPENED = 0;

    private static final int CLOSED = 1;

    private static final int NEW_WITHOUT_CHANGE = 2;

    private static final int NEW_WITH_CHANGE = 3;

    private static final int UPDATE_WITHOUT_CHANGE = 4;

    private static final int UPDATE_WITH_CHANGE = 5;

    private static final int SAVE_WITHOUT_CHANGE = 6;

    private static final int SAVE_WITH_CHANGE = 7;

    private JMenuItem jMenuItemFileSave = null;

    private JMenuItem jMenuItemFileClose = null;

    private JMenu jMenuTools = null;

    private JMenu jMenuWindow = null;

    private JPanel jPanelOperation = null;

    private JButton jButtonOk = null;

    private JButton jButtonCancel = null;

    private JMenuItem jMenuItemFileOpen = null;

    private JMenuItem jMenuItemFileCloseAll = null;

    private JMenuItem jMenuItemFileSaveAll = null;

    private JMenuItem jMenuItemFilePageSetup = null;

    private JMenuItem jMenuItemFilePrint = null;

    private JMenuItem jMenuItemFileImport = null;

    private JMenuItem jMenuItemFileProperties = null;

    private JMenu jMenuFileRecentFiles = null;

    private JSplitPane jSplitPane = null;

    private JMenuItem jMenuItemEditUndo = null;

    private JMenuItem jMenuItemEditRedo = null;

    private JMenuItem jMenuItemEditCut = null;

    private JMenuItem jMenuItemEditCopy = null;

    private JMenuItem jMenuItemEditPaste = null;

    private JMenuItem jMenuItemEditSelectAll = null;

    private JMenuItem jMenuItemEditFind = null;

    private JMenuItem jMenuItemEditFindNext = null;

    private JMenu jMenuView = null;

    private JMenu jMenuViewToolbars = null;

    private JCheckBoxMenuItem jCheckBoxMenuItemViewToolbarsFile = null;

    private JCheckBoxMenuItem jCheckBoxMenuItemViewToolbarsEdit = null;

    private JCheckBoxMenuItem jCheckBoxMenuItemViewToolbarsWindow = null;

    private JMenuItem jMenuItemViewStandard = null;

    private JMenuItem jMenuItemViewAdvanced = null;

    private JMenu jMenuProject = null;

    private JMenuItem jMenuItemProjectAdmin = null;

    private JMenuItem jMenuItemProjectChangeWorkspace = null;

    private JMenu jMenuProjectBuildTargets = null;

    private JCheckBoxMenuItem jCheckBoxMenuItemProjectBuildTargetsDebug = null;

    private JCheckBoxMenuItem jCheckBoxMenuItemProjectBuildTargetsRelease = null;

    private JMenuItem jMenuItemToolsToolChainConfiguration = null;

    private JMenuItem jMenuItemToolsClone = null;

    private JMenuItem jMenuItemToolsCodeScan = null;

    private JMenuItem jMenuItemWindowDisplaySide = null;

    private JMenuItem jMenuItemWindowDisplayTopBottom = null;

    private JMenuItem jMenuItemViewXML = null;

    private JMenuItem jMenuItemWindowTabView = null;

    private JMenuItem jMenuItemWindowSource = null;

    private JMenuItem jMenuItemWindowXML = null;

    private JMenuItem jMenuItemWindowPreferences = null;

    private JMenuItem jMenuItemHelpContents = null;

    private JMenuItem jMenuItemHelpIndex = null;

    private JMenuItem jMenuItemHelpSearch = null;

    private JMenuItem jMenuItemProjectInstallPackage = null;

    private JMenuItem jMenuItemProjectUpdatePackage = null;

    private JMenuItem jMenuItemProjectRemovePackage = null;

    //private JToolBar jToolBarFile = null;

    //private JToolBar jToolBarEdit = null;

    //private JToolBar jToolBarWindow = null;

    /**
     This method initializes jMenuBar 
     
     @return javax.swing.JMenuBar Main menu bar for the entire GUI
     
     **/
    private JMenuBar getjJMenuBar() {
        if (jMenuBar == null) {
            jMenuBar = new JMenuBar();
            jMenuBar.setPreferredSize(new java.awt.Dimension(0, 18));
            jMenuBar.add(getJMenuFile());
            jMenuBar.add(getJMenuEdit());
            jMenuBar.add(getJMenuView());
            jMenuBar.add(getJMenuProject());
            jMenuBar.add(getJMenuTools());
            jMenuBar.add(getJMenuWindow());
            jMenuBar.add(getJMenuHelp());
        }
        return jMenuBar;
    }

    /**
     This method initializes jSplitPane
     
     @return javax.swing.JSplitPane
     
     **/
    private JSplitPane getJSplitPane() {
        if (jSplitPane == null) {
            jSplitPane = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, getJTabbedPaneTree(), getJTabbedPaneEditor());
            jSplitPane.setBounds(new java.awt.Rectangle(0, 1, DataType.MAIN_FRAME_SPLIT_PANEL_PREFERRED_SIZE_WIDTH,
                                                        DataType.MAIN_FRAME_SPLIT_PANEL_PREFERRED_SIZE_HEIGHT));
            jSplitPane.addComponentListener(this);
        }
        return jSplitPane;
    }

    /**
     This method initializes jTabbedPaneEditor	
     
     @return javax.swing.JTabbedPane	
     
     */
    private JTabbedPane getJTabbedPaneEditor() {
        if (jTabbedPaneEditor == null) {
            jTabbedPaneEditor = new JTabbedPane();
            jTabbedPaneEditor.setBounds(new java.awt.Rectangle(DataType.MAIN_FRAME_EDITOR_PANEL_LOCATION_X,
                                                               DataType.MAIN_FRAME_EDITOR_PANEL_LOCATION_Y,
                                                               DataType.MAIN_FRAME_EDITOR_PANEL_PREFERRED_SIZE_WIDTH,
                                                               DataType.MAIN_FRAME_EDITOR_PANEL_PREFERRED_SIZE_HEIGHT));
            jTabbedPaneEditor
                             .setMinimumSize(new java.awt.Dimension(
                                                                    DataType.MAIN_FRAME_EDITOR_PANEL_PREFERRED_SIZE_WIDTH,
                                                                    DataType.MAIN_FRAME_EDITOR_PANEL_PREFERRED_SIZE_HEIGHT));
            jTabbedPaneEditor.addChangeListener(this);
            jTabbedPaneEditor.addTab("Module", null, getJDesktopPaneModule(), null);
            jTabbedPaneEditor.addTab("Package", null, getJDesktopPanePackage(), null);
            jTabbedPaneEditor.addTab("Platform", null, getJDesktopPanePlatform(), null);
        }
        return jTabbedPaneEditor;
    }

    /**
     This method initializes jTabbedPaneTree
     
     @return javax.swing.JTabbedPane	
     
     */
    private JTabbedPane getJTabbedPaneTree() {
        if (jTabbedPaneTree == null) {
            jTabbedPaneTree = new JTabbedPane();
            jTabbedPaneTree
                           .setPreferredSize(new java.awt.Dimension(
                                                                    DataType.MAIN_FRAME_TREE_PANEL_PREFERRED_SIZE_WIDTH,
                                                                    DataType.MAIN_FRAME_TREE_PANEL_PREFERRED_SIZE_HEIGHT));
            jTabbedPaneTree
                           .setMinimumSize(new java.awt.Dimension(DataType.MAIN_FRAME_TREE_PANEL_PREFERRED_SIZE_WIDTH,
                                                                  DataType.MAIN_FRAME_TREE_PANEL_PREFERRED_SIZE_HEIGHT));
            jTabbedPaneTree.addTab("Workspace Explorer", null, getJScrollPaneTree(), null);
        }
        return jTabbedPaneTree;
    }

    /**
     This method initializes jMenuFile 
     
     @return javax.swing.JMenu jMenuModule
     
     **/
    private JMenu getJMenuFile() {
        if (jMenuFile == null) {
            jMenuFile = new JMenu();
            jMenuFile.setText("File");
            jMenuFile.setMnemonic('F');
            jMenuFile.add(getJMenuItemFileNew());
            jMenuFile.add(getJMenuItemFileOpen());
            jMenuFile.add(getJMenuItemFileClose());
            jMenuFile.add(getJMenuItemFileCloseAll());
            jMenuFile.addSeparator();
            jMenuFile.add(getJMenuFileRecentFiles());
            jMenuFile.addSeparator();
            jMenuFile.add(getJMenuItemFileSave());
            jMenuFile.add(getJMenuItemFileSaveAs());
            jMenuFile.add(getJMenuItemFileSaveAll());
            jMenuFile.addSeparator();
            jMenuFile.add(getJMenuItemFilePageSetup());
            jMenuFile.add(getJMenuItemFilePrint());
            jMenuFile.addSeparator();
            jMenuFile.add(getJMenuItemFileImport());
            jMenuFile.addSeparator();
            jMenuFile.add(getJMenuItemFileProperties());
            jMenuFile.addSeparator();
            jMenuFile.add(getJMenuItemFileExit());
        }
        return jMenuFile;
    }

    /**
     This method initializes jMenuItemFileSaveAs 
     
     @return javax.swing.JMenuItem jMenuItemFileSaveAs
     
     **/
    private JMenuItem getJMenuItemFileSaveAs() {
        if (jMenuItemFileSaveAs == null) {
            jMenuItemFileSaveAs = new JMenuItem();
            jMenuItemFileSaveAs.setText("Save As...");
            jMenuItemFileSaveAs.setMnemonic('a');
            jMenuItemFileSaveAs.addActionListener(this);
            jMenuItemFileSaveAs.setEnabled(false);
        }
        return jMenuItemFileSaveAs;
    }

    /**
     This method initializes jMenuItemModuleExit 
     
     @return javax.swing.JMenuItem jMenuItemModuleExit
     
     **/
    private JMenuItem getJMenuItemFileExit() {
        if (jMenuItemFileExit == null) {
            jMenuItemFileExit = new JMenuItem();
            jMenuItemFileExit.setText("Exit");
            jMenuItemFileExit.setMnemonic('x');
            jMenuItemFileExit.addActionListener(this);
        }
        return jMenuItemFileExit;
    }

    /**
     This method initializes jMenuEdit 
     
     @return javax.swing.JMenu jMenuEdit
     
     **/
    private JMenu getJMenuEdit() {
        if (jMenuEdit == null) {
            jMenuEdit = new JMenu();
            jMenuEdit.setText("Edit");
            jMenuEdit.setMnemonic('E');
            jMenuEdit.add(getJMenuItemEditUndo());
            jMenuEdit.add(getJMenuItemEditRedo());
            jMenuEdit.addSeparator();
            jMenuEdit.add(getJMenuItemEditCut());
            jMenuEdit.add(getJMenuItemEditCopy());
            jMenuEdit.add(getJMenuItemEditPaste());
            jMenuEdit.add(getJMenuItemEditDelete());
            jMenuEdit.addSeparator();
            jMenuEdit.add(getJMenuItemEditSelectAll());
            jMenuEdit.add(getJMenuItemEditFind());
            jMenuEdit.add(getJMenuItemEditFindNext());
            jMenuEdit.addSeparator();
        }
        return jMenuEdit;
    }

    /**
     This method initializes jDesktopPane 
     
     @return javax.swing.JDesktopPane jDesktopPane
     
     **/
    private JDesktopPane getJDesktopPaneModule() {
        if (jDesktopPaneModule == null) {
            jDesktopPaneModule = new JDesktopPane();
            jDesktopPaneModule
                              .setBounds(new java.awt.Rectangle(DataType.MAIN_FRAME_EDITOR_PANEL_LOCATION_X,
                                                                DataType.MAIN_FRAME_EDITOR_PANEL_LOCATION_Y,
                                                                DataType.MAIN_FRAME_EDITOR_PANEL_PREFERRED_SIZE_WIDTH,
                                                                DataType.MAIN_FRAME_EDITOR_PANEL_PREFERRED_SIZE_HEIGHT));
            jDesktopPaneModule
                              .setMinimumSize(new java.awt.Dimension(
                                                                     DataType.MAIN_FRAME_EDITOR_PANEL_PREFERRED_SIZE_WIDTH,
                                                                     DataType.MAIN_FRAME_EDITOR_PANEL_PREFERRED_SIZE_HEIGHT));
            jDesktopPaneModule.setDesktopManager(iDesktopManager);
            jDesktopPaneModule.addComponentListener(this);
        }
        return jDesktopPaneModule;
    }

    /**
     This method initializes jDesktopPane 
     
     @return javax.swing.JDesktopPane jDesktopPane
     
     **/
    private JDesktopPane getJDesktopPanePackage() {
        if (jDesktopPanePackage == null) {
            jDesktopPanePackage = new JDesktopPane();
            jDesktopPanePackage
                               .setBounds(new java.awt.Rectangle(DataType.MAIN_FRAME_EDITOR_PANEL_LOCATION_X,
                                                                 DataType.MAIN_FRAME_EDITOR_PANEL_LOCATION_Y,
                                                                 DataType.MAIN_FRAME_EDITOR_PANEL_PREFERRED_SIZE_WIDTH,
                                                                 DataType.MAIN_FRAME_EDITOR_PANEL_PREFERRED_SIZE_HEIGHT));
            jDesktopPanePackage
                               .setMinimumSize(new java.awt.Dimension(
                                                                      DataType.MAIN_FRAME_EDITOR_PANEL_PREFERRED_SIZE_WIDTH,
                                                                      DataType.MAIN_FRAME_EDITOR_PANEL_PREFERRED_SIZE_HEIGHT));
            jDesktopPanePackage.setDesktopManager(iDesktopManager);
            jDesktopPanePackage.addComponentListener(this);
        }
        return jDesktopPanePackage;
    }

    /**
     This method initializes jDesktopPane 
     
     @return javax.swing.JDesktopPane jDesktopPane
     
     **/
    private JDesktopPane getJDesktopPanePlatform() {
        if (jDesktopPanePlatform == null) {
            jDesktopPanePlatform = new JDesktopPane();
            jDesktopPanePlatform
                                .setBounds(new java.awt.Rectangle(
                                                                  DataType.MAIN_FRAME_EDITOR_PANEL_LOCATION_X,
                                                                  DataType.MAIN_FRAME_EDITOR_PANEL_LOCATION_Y,
                                                                  DataType.MAIN_FRAME_EDITOR_PANEL_PREFERRED_SIZE_WIDTH,
                                                                  DataType.MAIN_FRAME_EDITOR_PANEL_PREFERRED_SIZE_HEIGHT));
            jDesktopPanePlatform
                                .setMinimumSize(new java.awt.Dimension(
                                                                       DataType.MAIN_FRAME_EDITOR_PANEL_PREFERRED_SIZE_WIDTH,
                                                                       DataType.MAIN_FRAME_EDITOR_PANEL_PREFERRED_SIZE_HEIGHT));
            jDesktopPanePlatform.setDesktopManager(iDesktopManager);
            jDesktopPanePlatform.addComponentListener(this);
        }
        return jDesktopPanePlatform;
    }

    /**
     This method initializes jScrollPaneTree 
     
     @return javax.swing.JScrollPane jScrollPaneTree
     
     **/
    private JScrollPane getJScrollPaneTree() {
        if (jScrollPaneTree == null) {
            jScrollPaneTree = new JScrollPane();
            //jScrollPaneTree.setBounds(new java.awt.Rectangle(0, 1, 290, 545));
            jScrollPaneTree
                           .setPreferredSize(new java.awt.Dimension(
                                                                    DataType.MAIN_FRAME_TREE_PANEL_PREFERRED_SIZE_WIDTH,
                                                                    DataType.MAIN_FRAME_TREE_PANEL_PREFERRED_SIZE_HEIGHT));
            jScrollPaneTree
                           .setMinimumSize(new java.awt.Dimension(
                                                                  DataType.MAIN_FRAME_TREE_PANEL_PREFERRED_SIZE_WIDTH / 2,
                                                                  DataType.MAIN_FRAME_TREE_PANEL_PREFERRED_SIZE_HEIGHT));
            jScrollPaneTree.setViewportView(getITree());
        }
        return jScrollPaneTree;
    }

    /**
     This method initializes iTree 
     
     @return org.tianocore.packaging.common.ui.ITree iTree
     
     **/
    private ITree getITree() {
        //
        //Before open a real module, use an empty root node for the tree
        //
        makeEmptyTree();
        return iTree;
    }

    /**
     This method initializes jMenuHelp 
     
     @return javax.swing.JMenu jMenuHelp
     
     **/
    private JMenu getJMenuHelp() {
        if (jMenuHelp == null) {
            jMenuHelp = new JMenu();
            jMenuHelp.setText("Help");
            jMenuHelp.add(getJMenuItemHelpContents());
            jMenuHelp.add(getJMenuItemHelpIndex());
            jMenuHelp.add(getJMenuItemHelpSearch());
            jMenuHelp.addSeparator();
            jMenuHelp.add(getJMenuItemHelpAbout());
        }
        return jMenuHelp;
    }

    /**
     This method initializes jMenuItemHelpAbout 
     
     @return javax.swing.JMenuItem jMenuItemHelpAbout
     
     **/
    private JMenuItem getJMenuItemHelpAbout() {
        if (jMenuItemHelpAbout == null) {
            jMenuItemHelpAbout = new JMenuItem();
            jMenuItemHelpAbout.setText("About...");
            jMenuItemHelpAbout.addActionListener(this);
        }
        return jMenuItemHelpAbout;
    }

    /**
     This method initializes jMenuItemEditDelete 
     
     @return javax.swing.JMenuItem jMenuItemEditDelete
     
     **/
    private JMenuItem getJMenuItemEditDelete() {
        if (jMenuItemEditDelete == null) {
            jMenuItemEditDelete = new JMenuItem();
            jMenuItemEditDelete.setText("Delete");
            jMenuItemEditDelete.setMnemonic('D');
            jMenuItemEditDelete.addActionListener(this);
            //
            //Disabled when no module is open
            //
            jMenuItemEditDelete.setEnabled(false);
        }
        return jMenuItemEditDelete;
    }

    /**
     This method initializes jPopupMenu 
     
     @return javax.swing.JPopupMenu jPopupMenu
     
     **/
    private JPopupMenu getJPopupMenu() {
        if (jPopupMenu == null) {
            jPopupMenu = new JPopupMenu();
            //
            //Add menu items of popup menu
            //
            jPopupMenu.add(getJMenuItemPopupAdd());
            jPopupMenu.add(getJMenuItemPopupUpdate());
            jPopupMenu.add(getJMenuItemPopupDelete());
            jPopupMenu.setBorder(new BevelBorder(BevelBorder.RAISED));
            jPopupMenu.addMouseListener(this);
        }
        return jPopupMenu;
    }

    /**
     This method initializes jMenuItemPopupAdd 
     
     @return javax.swing.JMenuItem jMenuItemPopupAdd
     
     **/
    private JMenuItem getJMenuItemPopupAdd() {
        if (jMenuItemPopupAdd == null) {
            jMenuItemPopupAdd = new JMenuItem();
            jMenuItemPopupAdd.setText("Add");
            jMenuItemPopupAdd.addActionListener(this);
            jMenuItemPopupAdd.setEnabled(false);
        }
        return jMenuItemPopupAdd;
    }

    /**
     This method initializes jMenuItemPopupUpdate 
     
     @return javax.swing.JMenuItem jMenuItemPopupUpdate
     
     **/
    private JMenuItem getJMenuItemPopupUpdate() {
        if (jMenuItemPopupUpdate == null) {
            jMenuItemPopupUpdate = new JMenuItem();
            jMenuItemPopupUpdate.setText("Update");
            jMenuItemPopupUpdate.addActionListener(this);
            jMenuItemPopupUpdate.setEnabled(false);
        }
        return jMenuItemPopupUpdate;
    }

    /**
     This method initializes jMenuItemPopupDelete 
     
     @return javax.swing.JMenuItem jMenuItemPopupDelete
     
     **/
    private JMenuItem getJMenuItemPopupDelete() {
        if (jMenuItemPopupDelete == null) {
            jMenuItemPopupDelete = new JMenuItem();
            jMenuItemPopupDelete.setText("Delete");
            jMenuItemPopupDelete.addActionListener(this);
            jMenuItemPopupDelete.setEnabled(false);
        }
        return jMenuItemPopupDelete;
    }

    /**
     This method initializes jMenuFileNew 
     
     @return javax.swing.JMenuItem jMenuFileNew
     
     **/
    private JMenuItem getJMenuItemFileNew() {
        if (jMenuItemFileNew == null) {
            jMenuItemFileNew = new JMenuItem();
            jMenuItemFileNew.setText("New...");
            jMenuItemFileNew.setMnemonic('N');
        }
        return jMenuItemFileNew;
    }

    /**
     This method initializes jMenuItemFileSave 
     
     @return javax.swing.JMenuItem jMenuItemModuleSave
     
     **/
    private JMenuItem getJMenuItemFileSave() {
        if (jMenuItemFileSave == null) {
            jMenuItemFileSave = new JMenuItem();
            jMenuItemFileSave.setText("Save");
            jMenuItemFileSave.setMnemonic('S');
            jMenuItemFileSave.addActionListener(this);
            jMenuItemFileSave.setEnabled(true);
        }
        return jMenuItemFileSave;
    }

    /**
     This method initializes jMenuItemModuleClose 
     
     @return javax.swing.JMenuItem jMenuItemModuleClose
     
     **/
    private JMenuItem getJMenuItemFileClose() {
        if (jMenuItemFileClose == null) {
            jMenuItemFileClose = new JMenuItem();
            jMenuItemFileClose.setText("Close");
            jMenuItemFileClose.setMnemonic('C');
            jMenuItemFileClose.setEnabled(false);
            jMenuItemFileClose.addActionListener(this);
        }
        return jMenuItemFileClose;
    }

    /**
     This method initializes jMenuTools
     Reserved 
     
     @return javax.swing.JMenu jMenuTools
     
     **/
    private JMenu getJMenuTools() {
        if (jMenuTools == null) {
            jMenuTools = new JMenu();
            jMenuTools.setText("Tools");
            jMenuTools.setMnemonic('T');
            jMenuTools.add(getJMenuItemToolsToolChainConfiguration());
            jMenuTools.addSeparator();
            jMenuTools.add(getJMenuItemToolsClone());
            jMenuTools.addSeparator();
            jMenuTools.add(getJMenuItemToolsCodeScan());
        }
        return jMenuTools;
    }

    /**
     This method initializes jMenuWindow 
     Reserved
     
     @return javax.swing.JMenu jMenuWindow
     
     **/
    private JMenu getJMenuWindow() {
        if (jMenuWindow == null) {
            jMenuWindow = new JMenu();
            jMenuWindow.setText("Window");
            jMenuWindow.setMnemonic('W');
            jMenuWindow.add(getJMenuItemWindowDisplaySide());
            jMenuWindow.add(getJMenuItemWindowDisplayTopBottom());
            jMenuWindow.addSeparator();
            jMenuWindow.add(getJMenuItemWindowTabView());
            jMenuWindow.addSeparator();
            jMenuWindow.add(getJMenuItemWindowSource());
            jMenuWindow.add(getJMenuItemWindowXML());
            jMenuWindow.addSeparator();
            jMenuWindow.add(getJMenuItemWindowPreferences());
        }
        return jMenuWindow;
    }

    /**
     This method initializes jPanelOperation 
     
     @return javax.swing.JPanel jPanelOperation
     
     **/
    private JPanel getJPanelOperation() {
        if (jPanelOperation == null) {
            jPanelOperation = new JPanel();
            jPanelOperation.setLayout(null);
            jPanelOperation.setBounds(new java.awt.Rectangle(295, 520, 500, 25));
            jPanelOperation.add(getJButtonOk(), null);
            jPanelOperation.add(getJButtonCancel(), null);
            jPanelOperation.setVisible(false);
        }
        return jPanelOperation;
    }

    /**
     This method initializes jButtonOk 
     
     @return javax.swing.JButton jButtonOk
     
     **/
    private JButton getJButtonOk() {
        if (jButtonOk == null) {
            jButtonOk = new JButton();
            jButtonOk.setBounds(new java.awt.Rectangle(395, 2, 90, 20));
            jButtonOk.setText("Ok");
            jButtonOk.setEnabled(false);
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
            jButtonCancel.setBounds(new java.awt.Rectangle(395, 2, 90, 20));
            jButtonCancel.setText("Cancel");
            jButtonCancel.setEnabled(false);
            jButtonCancel.addActionListener(this);
            jButtonCancel.setVisible(false);
        }
        return jButtonCancel;
    }

    /**
     This method initializes jMenuItemFileOpen	
     
     @return javax.swing.JMenuItem jMenuItemFileOpen
     */
    private JMenuItem getJMenuItemFileOpen() {
        if (jMenuItemFileOpen == null) {
            jMenuItemFileOpen = new JMenuItem();
            jMenuItemFileOpen.setText("Open...");
            jMenuItemFileOpen.setMnemonic('O');
            jMenuItemFileOpen.addActionListener(this);
        }
        return jMenuItemFileOpen;
    }

    /**
     This method initializes jMenuItemFileCloseAll	
     
     @return javax.swing.JMenuItem jMenuItemFileOpen
     */
    private JMenuItem getJMenuItemFileCloseAll() {
        if (jMenuItemFileCloseAll == null) {
            jMenuItemFileCloseAll = new JMenuItem();
            jMenuItemFileCloseAll.setText("Close All");
            jMenuItemFileCloseAll.setEnabled(true);
            jMenuItemFileCloseAll.addActionListener(this);
        }
        return jMenuItemFileCloseAll;
    }

    /**
     This method initializes jMenuItemFileSaveAll	
     
     @return javax.swing.JMenuItem jMenuItemFileSaveAll
     */
    private JMenuItem getJMenuItemFileSaveAll() {
        if (jMenuItemFileSaveAll == null) {
            jMenuItemFileSaveAll = new JMenuItem();
            jMenuItemFileSaveAll.setText("Save All");
            jMenuItemFileSaveAll.setMnemonic('v');
            jMenuItemFileSaveAll.setEnabled(false);
            jMenuItemFileSaveAll.addActionListener(this);
        }
        return jMenuItemFileSaveAll;
    }

    /**
     This method initializes jMenuItemFilePageSetup	
     
     @return javax.swing.JMenuItem	
     */
    private JMenuItem getJMenuItemFilePageSetup() {
        if (jMenuItemFilePageSetup == null) {
            jMenuItemFilePageSetup = new JMenuItem();
            jMenuItemFilePageSetup.setText("Page Setup");
            jMenuItemFilePageSetup.setMnemonic('u');
            jMenuItemFilePageSetup.setEnabled(false);
            jMenuItemFilePageSetup.addActionListener(this);
        }
        return jMenuItemFilePageSetup;
    }

    /**
     This method initializes jMenuItemFilePrint	
     
     @return javax.swing.JMenuItem	
     */
    private JMenuItem getJMenuItemFilePrint() {
        if (jMenuItemFilePrint == null) {
            jMenuItemFilePrint = new JMenuItem();
            jMenuItemFilePrint.setText("Print");
            jMenuItemFilePrint.setMnemonic('P');
            jMenuItemFilePrint.setEnabled(false);
            jMenuItemFilePrint.addActionListener(this);
        }
        return jMenuItemFilePrint;
    }

    /**
     This method initializes jMenuItemFileImport	
     
     @return javax.swing.JMenuItem	
     */
    private JMenuItem getJMenuItemFileImport() {
        if (jMenuItemFileImport == null) {
            jMenuItemFileImport = new JMenuItem();
            jMenuItemFileImport.setText("Import");
            jMenuItemFileImport.setMnemonic('I');
            jMenuItemFileImport.setEnabled(false);
            jMenuItemFileImport.addActionListener(this);
        }
        return jMenuItemFileImport;
    }

    /**
     * This method initializes jMenuItemFileProperties	
     * 	
     * @return javax.swing.JMenuItem	
     */
    private JMenuItem getJMenuItemFileProperties() {
        if (jMenuItemFileProperties == null) {
            jMenuItemFileProperties = new JMenuItem();
            jMenuItemFileProperties.setText("Properties");
            jMenuItemFileProperties.setMnemonic('t');
            jMenuItemFileProperties.setEnabled(false);
            jMenuItemFileProperties.addActionListener(this);
        }
        return jMenuItemFileProperties;
    }

    /**
     * This method initializes jMenuFileRecentFiles	
     * 	
     * @return javax.swing.JMenu	
     */
    private JMenu getJMenuFileRecentFiles() {
        if (jMenuFileRecentFiles == null) {
            jMenuFileRecentFiles = new JMenu();
            jMenuFileRecentFiles.setText("Recent Files");
            jMenuFileRecentFiles.setMnemonic('F');
            jMenuFileRecentFiles.setEnabled(false);
            jMenuFileRecentFiles.addActionListener(this);
        }
        return jMenuFileRecentFiles;
    }

    /**
     This method initializes jMenuItemEditUndo	
     
     @return javax.swing.JMenuItem	
     */
    private JMenuItem getJMenuItemEditUndo() {
        if (jMenuItemEditUndo == null) {
            jMenuItemEditUndo = new JMenuItem();
            jMenuItemEditUndo.setText("Undo");
            jMenuItemEditUndo.setMnemonic('U');
            jMenuItemEditUndo.setEnabled(false);
            jMenuItemEditUndo.addActionListener(this);
        }
        return jMenuItemEditUndo;
    }

    /**
     This method initializes jMenuItemEditRedo	
     
     @return javax.swing.JMenuItem	
     */
    private JMenuItem getJMenuItemEditRedo() {
        if (jMenuItemEditRedo == null) {
            jMenuItemEditRedo = new JMenuItem();
            jMenuItemEditRedo.setText("Redo");
            jMenuItemEditRedo.setMnemonic('R');
            jMenuItemEditRedo.setEnabled(false);
            jMenuItemEditRedo.addActionListener(this);
        }
        return jMenuItemEditRedo;
    }

    /**
     This method initializes jMenuItemEditCut	
     
     @return javax.swing.JMenuItem	
     */
    private JMenuItem getJMenuItemEditCut() {
        if (jMenuItemEditCut == null) {
            jMenuItemEditCut = new JMenuItem();
            jMenuItemEditCut.setText("Cut");
            jMenuItemEditCut.setMnemonic('t');
            jMenuItemEditCut.setEnabled(false);
            jMenuItemEditCut.addActionListener(this);
        }
        return jMenuItemEditCut;
    }

    /**
     This method initializes jMenuItemEditCopy	
     
     @return javax.swing.JMenuItem	
     */
    private JMenuItem getJMenuItemEditCopy() {
        if (jMenuItemEditCopy == null) {
            jMenuItemEditCopy = new JMenuItem();
            jMenuItemEditCopy.setText("Copy");
            jMenuItemEditCopy.setMnemonic('C');
            jMenuItemEditCopy.setEnabled(false);
            jMenuItemEditCopy.addActionListener(this);
        }
        return jMenuItemEditCopy;
    }

    /**
     This method initializes jMenuItemEditPaste	
     
     return javax.swing.JMenuItem	
     */
    private JMenuItem getJMenuItemEditPaste() {
        if (jMenuItemEditPaste == null) {
            jMenuItemEditPaste = new JMenuItem();
            jMenuItemEditPaste.setText("Paste");
            jMenuItemEditPaste.setMnemonic('P');
            jMenuItemEditPaste.setEnabled(false);
            jMenuItemEditPaste.addActionListener(this);
        }
        return jMenuItemEditPaste;
    }

    /**
     This method initializes jMenuItem	
     
     @return javax.swing.JMenuItem	
     */
    private JMenuItem getJMenuItemEditSelectAll() {
        if (jMenuItemEditSelectAll == null) {
            jMenuItemEditSelectAll = new JMenuItem();
            jMenuItemEditSelectAll.setText("Select All");
            jMenuItemEditSelectAll.setMnemonic('A');
            jMenuItemEditSelectAll.setEnabled(false);
            jMenuItemEditSelectAll.addActionListener(this);
        }
        return jMenuItemEditSelectAll;
    }

    /**
     This method initializes jMenuItemEditFind	
     
     @return javax.swing.JMenuItem	
     */
    private JMenuItem getJMenuItemEditFind() {
        if (jMenuItemEditFind == null) {
            jMenuItemEditFind = new JMenuItem();
            jMenuItemEditFind.setText("Find");
            jMenuItemEditFind.setMnemonic('F');
            jMenuItemEditFind.setEnabled(false);
            jMenuItemEditFind.addActionListener(this);
        }
        return jMenuItemEditFind;
    }

    /**
     This method initializes jMenuItemEditFindNext	
     
     @return javax.swing.JMenuItem	
     
     */
    private JMenuItem getJMenuItemEditFindNext() {
        if (jMenuItemEditFindNext == null) {
            jMenuItemEditFindNext = new JMenuItem();
            jMenuItemEditFindNext.setText("Find Next");
            jMenuItemEditFindNext.setMnemonic('n');
            jMenuItemEditFindNext.setEnabled(false);
            jMenuItemEditFindNext.addActionListener(this);
        }
        return jMenuItemEditFindNext;
    }

    /**
     This method initializes jMenuView	
     
     @return javax.swing.JMenu	
     
     */
    private JMenu getJMenuView() {
        if (jMenuView == null) {
            jMenuView = new JMenu();
            jMenuView.setText("View");
            jMenuView.setMnemonic('V');
            jMenuView.add(getJMenuViewToolbars());
            jMenuView.add(getJMenuItemViewAdvanced());
            jMenuView.add(getJMenuItemViewStandard());
            jMenuView.add(getJMenuItemViewXML());
        }
        return jMenuView;
    }

    /**
     This method initializes jMenuViewToolbars	
     
     @return javax.swing.JMenu	
     
     */
    private JMenu getJMenuViewToolbars() {
        if (jMenuViewToolbars == null) {
            jMenuViewToolbars = new JMenu();
            jMenuViewToolbars.setText("Toolbars");
            jMenuViewToolbars.setMnemonic('T');
            jMenuViewToolbars.add(getJCheckBoxMenuItemViewToolbarsFile());
            jMenuViewToolbars.add(getJCheckBoxMenuItemViewToolbarsEdit());
            jMenuViewToolbars.add(getJCheckBoxMenuItemViewToolbarsWindow());
        }
        return jMenuViewToolbars;
    }

    /**
     This method initializes jCheckBoxMenuItemViewToolbarsFile	
     
     @return javax.swing.JCheckBoxMenuItem	
     
     */
    private JCheckBoxMenuItem getJCheckBoxMenuItemViewToolbarsFile() {
        if (jCheckBoxMenuItemViewToolbarsFile == null) {
            jCheckBoxMenuItemViewToolbarsFile = new JCheckBoxMenuItem();
            jCheckBoxMenuItemViewToolbarsFile.setText("File");
            jCheckBoxMenuItemViewToolbarsFile.setEnabled(false);
            jCheckBoxMenuItemViewToolbarsFile.addActionListener(this);
        }
        return jCheckBoxMenuItemViewToolbarsFile;
    }

    /**
     This method initializes jCheckBoxMenuItemViewToolbarsEdit	
     
     @return javax.swing.JCheckBoxMenuItem	
     
     */
    private JCheckBoxMenuItem getJCheckBoxMenuItemViewToolbarsEdit() {
        if (jCheckBoxMenuItemViewToolbarsEdit == null) {
            jCheckBoxMenuItemViewToolbarsEdit = new JCheckBoxMenuItem();
            jCheckBoxMenuItemViewToolbarsEdit.setText("Edit");
            jCheckBoxMenuItemViewToolbarsEdit.setEnabled(false);
            jCheckBoxMenuItemViewToolbarsEdit.addActionListener(this);
        }
        return jCheckBoxMenuItemViewToolbarsEdit;
    }

    /**
     This method initializes jCheckBoxMenuItemViewToolbarsWindow	
     
     @return javax.swing.JCheckBoxMenuItem	
     
     */
    private JCheckBoxMenuItem getJCheckBoxMenuItemViewToolbarsWindow() {
        if (jCheckBoxMenuItemViewToolbarsWindow == null) {
            jCheckBoxMenuItemViewToolbarsWindow = new JCheckBoxMenuItem();
            jCheckBoxMenuItemViewToolbarsWindow.setText("Window");
            jCheckBoxMenuItemViewToolbarsWindow.setEnabled(false);
            jCheckBoxMenuItemViewToolbarsWindow.addActionListener(this);
        }
        return jCheckBoxMenuItemViewToolbarsWindow;
    }

    /**
     This method initializes jMenuItemStandard	
     
     @return javax.swing.JMenuItem	
     
     */
    private JMenuItem getJMenuItemViewStandard() {
        if (jMenuItemViewStandard == null) {
            jMenuItemViewStandard = new JMenuItem();
            jMenuItemViewStandard.setText("Standard");
            jMenuItemViewStandard.setMnemonic('S');
            jMenuItemViewStandard.setEnabled(false);
            jMenuItemViewStandard.addActionListener(this);
        }
        return jMenuItemViewStandard;
    }

    /**
     This method initializes jMenuItemAdvanced	
     
     @return javax.swing.JMenuItem	
     
     */
    private JMenuItem getJMenuItemViewAdvanced() {
        if (jMenuItemViewAdvanced == null) {
            jMenuItemViewAdvanced = new JMenuItem();
            jMenuItemViewAdvanced.setText("Advanced");
            jMenuItemViewAdvanced.setMnemonic('A');
            jMenuItemViewAdvanced.setEnabled(false);
            jMenuItemViewAdvanced.addActionListener(this);
        }
        return jMenuItemViewAdvanced;
    }

    /**
     This method initializes jMenuProject	
     
     @return javax.swing.JMenu	
     
     */
    private JMenu getJMenuProject() {
        if (jMenuProject == null) {
            jMenuProject = new JMenu();
            jMenuProject.setText("Project");
            jMenuProject.setMnemonic('P');
            jMenuProject.add(getJMenuItemProjectAdmin());
            jMenuProject.addSeparator();
            jMenuProject.add(getJMenuItemProjectChangeWorkspace());
            jMenuProject.addSeparator();
            jMenuProject.add(getJMenuItemProjectInstallPackage());
            jMenuProject.add(getJMenuItemProjectUpdatePackage());
            jMenuProject.add(getJMenuItemProjectRemovePackage());
            jMenuProject.addSeparator();
            jMenuProject.add(getJMenuProjectBuildTargets());
        }
        return jMenuProject;
    }

    /**
     This method initializes jMenuItemProjectAdmin	
     
     @return javax.swing.JMenuItem	
     
     */
    private JMenuItem getJMenuItemProjectAdmin() {
        if (jMenuItemProjectAdmin == null) {
            jMenuItemProjectAdmin = new JMenuItem();
            jMenuItemProjectAdmin.setText("Admin...");
            jMenuItemProjectAdmin.setMnemonic('A');
            jMenuItemProjectAdmin.setEnabled(false);
            jMenuItemProjectAdmin.addActionListener(this);
        }
        return jMenuItemProjectAdmin;
    }

    /**
     This method initializes jMenuItemProjectChangeWorkspace	
     
     @return javax.swing.JMenuItem	
     
     */
    private JMenuItem getJMenuItemProjectChangeWorkspace() {
        if (jMenuItemProjectChangeWorkspace == null) {
            jMenuItemProjectChangeWorkspace = new JMenuItem();
            jMenuItemProjectChangeWorkspace.setText("Change WORKSPACE...");
            jMenuItemProjectChangeWorkspace.setMnemonic('W');
            jMenuItemProjectChangeWorkspace.setEnabled(true);
            jMenuItemProjectChangeWorkspace.addActionListener(this);
        }
        return jMenuItemProjectChangeWorkspace;
    }

    /**
     This method initializes jMenuProjectBuildTargets	
     
     @return javax.swing.JMenu	
     
     */
    private JMenu getJMenuProjectBuildTargets() {
        if (jMenuProjectBuildTargets == null) {
            jMenuProjectBuildTargets = new JMenu();
            jMenuProjectBuildTargets.setText("Build Targets");
            jMenuProjectBuildTargets.setMnemonic('T');
            jMenuProjectBuildTargets.add(getJCheckBoxMenuItemProjectBuildTargetsDebug());
            jMenuProjectBuildTargets.add(getJCheckBoxMenuItemProjectBuildTargetsRelease());
        }
        return jMenuProjectBuildTargets;
    }

    /**
     This method initializes jCheckBoxMenuItemProjectBuildTargetsDebug	
     
     @return javax.swing.JCheckBoxMenuItem	
     
     */
    private JCheckBoxMenuItem getJCheckBoxMenuItemProjectBuildTargetsDebug() {
        if (jCheckBoxMenuItemProjectBuildTargetsDebug == null) {
            jCheckBoxMenuItemProjectBuildTargetsDebug = new JCheckBoxMenuItem();
            jCheckBoxMenuItemProjectBuildTargetsDebug.setText("Debug");
            jCheckBoxMenuItemProjectBuildTargetsDebug.setEnabled(false);
        }
        return jCheckBoxMenuItemProjectBuildTargetsDebug;
    }

    /**
     This method initializes jCheckBoxMenuItemProjectBuildTargetsRelease	
     
     @return javax.swing.JCheckBoxMenuItem	
     
     */
    private JCheckBoxMenuItem getJCheckBoxMenuItemProjectBuildTargetsRelease() {
        if (jCheckBoxMenuItemProjectBuildTargetsRelease == null) {
            jCheckBoxMenuItemProjectBuildTargetsRelease = new JCheckBoxMenuItem();
            jCheckBoxMenuItemProjectBuildTargetsRelease.setText("Release");
            jCheckBoxMenuItemProjectBuildTargetsRelease.setEnabled(false);
        }
        return jCheckBoxMenuItemProjectBuildTargetsRelease;
    }

    /**
     This method initializes jMenuItemToolsToolChainConfiguration	
     
     @return javax.swing.JMenuItem	
     
     */
    private JMenuItem getJMenuItemToolsToolChainConfiguration() {
        if (jMenuItemToolsToolChainConfiguration == null) {
            jMenuItemToolsToolChainConfiguration = new JMenuItem();
            jMenuItemToolsToolChainConfiguration.setText("Tool Chain Configuration...");
            jMenuItemToolsToolChainConfiguration.setMnemonic('C');
            jMenuItemToolsToolChainConfiguration.addActionListener(this);
        }
        return jMenuItemToolsToolChainConfiguration;
    }

    /**
     This method initializes jMenuItemToolsClone	
     
     @return javax.swing.JMenuItem	
     
     */
    private JMenuItem getJMenuItemToolsClone() {
        if (jMenuItemToolsClone == null) {
            jMenuItemToolsClone = new JMenuItem();
            jMenuItemToolsClone.setText("Clone...");
            jMenuItemToolsClone.setMnemonic('l');
            jMenuItemToolsClone.setEnabled(false);
            jMenuItemToolsClone.addActionListener(this);
        }
        return jMenuItemToolsClone;
    }

    /**
     This method initializes jMenuItemToolsCodeScan	
     
     @return javax.swing.JMenuItem	
     
     */
    private JMenuItem getJMenuItemToolsCodeScan() {
        if (jMenuItemToolsCodeScan == null) {
            jMenuItemToolsCodeScan = new JMenuItem();
            jMenuItemToolsCodeScan.setText("Code Scan...");
            jMenuItemToolsCodeScan.setMnemonic('S');
            jMenuItemToolsCodeScan.setEnabled(false);
            jMenuItemToolsCodeScan.addActionListener(this);
        }
        return jMenuItemToolsCodeScan;
    }

    /**
     This method initializes jMenuItemWindowSplitVertical	
     
     @return javax.swing.JMenuItem	
     
     */
    private JMenuItem getJMenuItemWindowDisplaySide() {
        if (jMenuItemWindowDisplaySide == null) {
            jMenuItemWindowDisplaySide = new JMenuItem();
            jMenuItemWindowDisplaySide.setText("Display Side by Side");
            jMenuItemWindowDisplaySide.setMnemonic('S');
            jMenuItemWindowDisplaySide.setEnabled(false);
            jMenuItemWindowDisplaySide.addActionListener(this);
        }
        return jMenuItemWindowDisplaySide;
    }

    /**
     This method initializes jMenuItemWindowSplitHorizontal	
     
     @return javax.swing.JMenuItem	
     
     */
    private JMenuItem getJMenuItemWindowDisplayTopBottom() {
        if (jMenuItemWindowDisplayTopBottom == null) {
            jMenuItemWindowDisplayTopBottom = new JMenuItem();
            jMenuItemWindowDisplayTopBottom.setText("Display Top and Bottom");
            jMenuItemWindowDisplayTopBottom.setMnemonic('B');
            jMenuItemWindowDisplayTopBottom.setEnabled(false);
            jMenuItemWindowDisplayTopBottom.addActionListener(this);
        }
        return jMenuItemWindowDisplayTopBottom;
    }

    /**
     This method initializes jMenuItemViewXML	
     
     @return javax.swing.JMenuItem	
     
     */
    private JMenuItem getJMenuItemViewXML() {
        if (jMenuItemViewXML == null) {
            jMenuItemViewXML = new JMenuItem();
            jMenuItemViewXML.setText("XML");
            jMenuItemViewXML.setMnemonic('X');
            jMenuItemViewXML.setEnabled(false);
            jMenuItemViewXML.addActionListener(this);
        }
        return jMenuItemViewXML;
    }

    /**
     This method initializes jMenuItemWindowTabView	
     
     @return javax.swing.JMenuItem	
     
     */
    private JMenuItem getJMenuItemWindowTabView() {
        if (jMenuItemWindowTabView == null) {
            jMenuItemWindowTabView = new JMenuItem();
            jMenuItemWindowTabView.setText("Tab View");
            jMenuItemWindowTabView.setMnemonic('T');
            jMenuItemWindowTabView.setEnabled(false);
            jMenuItemWindowTabView.addActionListener(this);
        }
        return jMenuItemWindowTabView;
    }

    /**
     This method initializes jMenuItemWindowSource	
     
     @return javax.swing.JMenuItem	
     
     */
    private JMenuItem getJMenuItemWindowSource() {
        if (jMenuItemWindowSource == null) {
            jMenuItemWindowSource = new JMenuItem();
            jMenuItemWindowSource.setText("Source");
            jMenuItemWindowSource.setMnemonic('S');
            jMenuItemWindowSource.setEnabled(false);
            jMenuItemWindowSource.addActionListener(this);
        }
        return jMenuItemWindowSource;
    }

    /**
     This method initializes jMenuItemWindowXML	
     
     @return javax.swing.JMenuItem	
     
     */
    private JMenuItem getJMenuItemWindowXML() {
        if (jMenuItemWindowXML == null) {
            jMenuItemWindowXML = new JMenuItem();
            jMenuItemWindowXML.setText("XML");
            jMenuItemWindowXML.setMnemonic('X');
            jMenuItemWindowXML.setEnabled(false);
            jMenuItemWindowXML.addActionListener(this);
        }
        return jMenuItemWindowXML;
    }

    /**
     This method initializes jMenuItemWindowPreferences	
     
     @return javax.swing.JMenuItem	
     
     */
    private JMenuItem getJMenuItemWindowPreferences() {
        if (jMenuItemWindowPreferences == null) {
            jMenuItemWindowPreferences = new JMenuItem();
            jMenuItemWindowPreferences.setText("Preferences");
            jMenuItemWindowPreferences.setMnemonic('P');
            jMenuItemWindowPreferences.setEnabled(false);
            jMenuItemWindowPreferences.addActionListener(this);
        }
        return jMenuItemWindowPreferences;
    }

    /**
     This method initializes jMenuItemHelpContents	
     
     @return javax.swing.JMenuItem	
     
     */
    private JMenuItem getJMenuItemHelpContents() {
        if (jMenuItemHelpContents == null) {
            jMenuItemHelpContents = new JMenuItem();
            jMenuItemHelpContents.setText("Contents");
            jMenuItemHelpContents.setMnemonic('C');
            jMenuItemHelpContents.setEnabled(false);
            jMenuItemHelpContents.addActionListener(this);
        }
        return jMenuItemHelpContents;
    }

    /**
     This method initializes jMenuItemHelpIndex	
     
     @return javax.swing.JMenuItem	
     
     */
    private JMenuItem getJMenuItemHelpIndex() {
        if (jMenuItemHelpIndex == null) {
            jMenuItemHelpIndex = new JMenuItem();
            jMenuItemHelpIndex.setText("Index");
            jMenuItemHelpIndex.setMnemonic('I');
            jMenuItemHelpIndex.setEnabled(false);
            jMenuItemHelpIndex.addActionListener(this);
        }
        return jMenuItemHelpIndex;
    }

    /**
     This method initializes jMenuItemHelpSearch	
     
     @return javax.swing.JMenuItem	
     
     */
    private JMenuItem getJMenuItemHelpSearch() {
        if (jMenuItemHelpSearch == null) {
            jMenuItemHelpSearch = new JMenuItem();
            jMenuItemHelpSearch.setText("Search");
            jMenuItemHelpSearch.setMnemonic('S');
            jMenuItemHelpSearch.setEnabled(false);
            jMenuItemHelpSearch.addActionListener(this);
        }
        return jMenuItemHelpSearch;
    }

    /**
     * This method initializes jToolBar	
     * 	
     * @return javax.swing.JToolBar	
     */
    //	private JToolBar getJToolBarFile() {
    //		if (jToolBarFile == null) {
    //			jToolBarFile = new JToolBar();
    //			jToolBarFile.setFloatable(false);
    //		}
    //		return jToolBarFile;
    //	}
    /**
     * This method initializes jToolBarEdit	
     * 	
     * @return javax.swing.JToolBar	
     */
    //	private JToolBar getJToolBarEdit() {
    //		if (jToolBarEdit == null) {
    //			jToolBarEdit = new JToolBar();
    //			jToolBarEdit.setFloatable(false);
    //		}
    //		return jToolBarEdit;
    //	}
    /**
     * This method initializes jMenuItemToolsInstallPackage	
     * 	
     * @return javax.swing.JMenuItem	
     */
    private JMenuItem getJMenuItemProjectInstallPackage() {
        if (jMenuItemProjectInstallPackage == null) {
            jMenuItemProjectInstallPackage = new JMenuItem();
            jMenuItemProjectInstallPackage.setText("Install Distribution Package");
            jMenuItemProjectInstallPackage.setMnemonic('I');
            jMenuItemProjectInstallPackage.setEnabled(false);
            jMenuItemProjectInstallPackage.addActionListener(this);
        }
        return jMenuItemProjectInstallPackage;
    }

    /**
     * This method initializes jMenuItemToolsUpdatePackage	
     * 	
     * @return javax.swing.JMenuItem	
     */
    private JMenuItem getJMenuItemProjectUpdatePackage() {
        if (jMenuItemProjectUpdatePackage == null) {
            jMenuItemProjectUpdatePackage = new JMenuItem();
            jMenuItemProjectUpdatePackage.setText("Update Disstribution Package");
            jMenuItemProjectUpdatePackage.setMnemonic('U');
            jMenuItemProjectUpdatePackage.setEnabled(false);
            jMenuItemProjectUpdatePackage.addActionListener(this);
        }
        return jMenuItemProjectUpdatePackage;
    }

    /**
     * This method initializes jMenuItemRemovePackage	
     * 	
     * @return javax.swing.JMenuItem	
     */
    private JMenuItem getJMenuItemProjectRemovePackage() {
        if (jMenuItemProjectRemovePackage == null) {
            jMenuItemProjectRemovePackage = new JMenuItem();
            jMenuItemProjectRemovePackage.setText("Remove Distribution Package");
            jMenuItemProjectRemovePackage.setMnemonic('R');
            jMenuItemProjectRemovePackage.setEnabled(false);
            jMenuItemProjectRemovePackage.addActionListener(this);
        }
        return jMenuItemProjectRemovePackage;
    }

    /* (non-Javadoc)
     * @see org.tianocore.packaging.common.ui.IFrame#main(java.lang.String[])
     *
     * Main class, start the GUI
     * 
     */
    public static void main(String[] args) {
        FrameworkWizardUI module = new FrameworkWizardUI();
        module.setVisible(true);
    }

    /**
     This is the default constructor
     
     **/
    public FrameworkWizardUI() {
        super();
        init();
    }

    /**
     This method initializes this
     
     
     **/
    private void init() {
        //
        // Check if exists WORKSPACE
        // 
        //
        if (!ws.checkCurrentWorkspace()) {
            JOptionPane.showConfirmDialog(null, "You haven't a workspace yet. Please setup first.", "Warning",
                                          JOptionPane.DEFAULT_OPTION, JOptionPane.WARNING_MESSAGE);
            this.dispose();
            System.exit(0);
        }

        this.setSize(DataType.MAIN_FRAME_PREFERRED_SIZE_WIDTH, DataType.MAIN_FRAME_PREFERRED_SIZE_HEIGHT);
        this.setResizable(true);
        this.setJMenuBar(getjJMenuBar());
        this.addComponentListener(this);
        this.getCompontentsFromFrameworkDatabase();
        this.setContentPane(getJContentPane());
        this.setTitle(windowTitle + "- [" + ws.getCurrentWorkspace() + "]");
        this.setExitType(1);
        this.centerWindow();
    }

    /**
     This method initializes jContentPane
     
     @return javax.swing.JPanel jContentPane
     
     **/
    private JPanel getJContentPane() {
        if (jContentPane == null) {
            jContentPane = new JPanel();
            jContentPane.setLayout(null);
            jContentPane.add(getJPanelOperation(), null);
            jContentPane.add(getJSplitPane(), null);
            jContentPane.add(getJPopupMenu(), null);
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
        //
        // Operations of Menu
        //
        if (arg0.getSource() == jMenuItemFileNew) {
            this.fileNew();
        }

        if (arg0.getSource() == jMenuItemFileOpen) {
            this.open();
        }

        if (arg0.getSource() == jMenuItemFileClose) {
            this.close();
        }

        if (arg0.getSource() == jMenuItemFileCloseAll) {
            this.closeAll();
        }

        if (arg0.getSource() == jMenuItemFileSave) {
            this.save();
        }

        if (arg0.getSource() == jMenuItemFileSaveAs) {
            this.saveAs();
        }

        if (arg0.getSource() == jMenuItemFileSaveAll) {
            this.saveAll();
        }

        if (arg0.getSource() == jMenuItemFileExit) {
            this.exit();
        }

        if (arg0.getSource() == jMenuItemHelpAbout) {
            About a = new About();
            a.setEdited(false);
        }
    }

    /**
     Open file
     
     @param intOperationType Open - 1 or Save - 2
     @param intFileType Msa - 1, Mbd - 2, Lmsa - 3, Lmbd - 4
     @return opened file path
     
     **/
    private void openFile(int intOperationType, int intFileType) {
        String strCurrentPath = "";
        if (this.currentModule == "") {
            strCurrentPath = ws.getCurrentWorkspace();
        } else {
            strCurrentPath = this.currentModule.substring(0, this.currentModule.lastIndexOf(Tools.FIEL_SEPARATOR));
        }

        JFileChooser fc = new JFileChooser(strCurrentPath);
        fc.setAcceptAllFileFilterUsed(false);
        switch (intOperationType) {
        case 1:
            fc.setDialogTitle("Open");
            break;
        case 2:
            fc.setDialogTitle("Save As");
            break;
        }
        //
        // Config File Filter via different file types
        //
        switch (intFileType) {
        case 1:
            fc.addChoosableFileFilter(new IFileFilter("msa"));
            break;
        case 2:
            fc.addChoosableFileFilter(new IFileFilter("mbd"));
            break;
        case 3:
            fc.addChoosableFileFilter(new IFileFilter("msa"));
            break;
        case 4:
            fc.addChoosableFileFilter(new IFileFilter("mbd"));
            break;
        }

        int result = fc.showOpenDialog(new JPanel());
        //
        // Open relevanf file after click "OK"
        //
        if (result == JFileChooser.APPROVE_OPTION) {
            switch (intOperationType) {
            case 1:
                closeCurrentModule();
                switch (intFileType) {
                case 1:
                    String strMsaFilePath = fc.getSelectedFile().getPath();
                    try {
                        openMsaFile(strMsaFilePath);
                    } catch (IOException e) {
                        Log.err("Open Msa " + strMsaFilePath, e.getMessage());
                        return;
                    } catch (XmlException e) {
                        Log.err("Open Msa " + strMsaFilePath, e.getMessage());
                        return;
                    } catch (Exception e) {
                        Log.err("Open Msa " + strMsaFilePath, "Invalid file type");
                        return;
                    }
                    break;
                }
            case 2:
                switch (intFileType) {
                case 1:
                    this.saveFileName = fc.getSelectedFile().getPath();
                    break;
                case 2:
                    this.saveFileName = fc.getSelectedFile().getPath();
                    break;
                case 3:
                    this.saveFileName = fc.getSelectedFile().getPath();
                    break;
                case 4:
                    this.saveFileName = fc.getSelectedFile().getPath();
                    break;
                }
                break;
            }
        } else {
            if (intOperationType == 2) {
                this.saveFileName = "";
            }
        }
    }

    /**
     Open specificed Msa file and read its content
     
     @param strMsaFilePath The input data of Msa File Path
     
     **/
    private ModuleSurfaceAreaDocument.ModuleSurfaceArea openMsaFile(String strMsaFilePath) throws IOException,
                                                                                          XmlException, Exception {
        Log.log("Open Msa", strMsaFilePath);
        File msaFile = new File(strMsaFilePath);
        xmlMsaDoc = (ModuleSurfaceAreaDocument) XmlObject.Factory.parse(msaFile);
        this.currentModule = strMsaFilePath;
        this.saveFileName = strMsaFilePath;
        this.currentModuleType = 1;

        return xmlMsaDoc.getModuleSurfaceArea();
    }

    /**
     Open specificed Spd file and read its content
     
     @param strSpdFilePath The input data of Spd File Path
     
     **/
    private PackageSurfaceAreaDocument.PackageSurfaceArea openSpdFile(String strSpdFilePath) throws IOException,
                                                                                            XmlException, Exception {
        Log.log("Open Spd", strSpdFilePath);
        File spdFile = new File(strSpdFilePath);
        xmlSpdDoc = (PackageSurfaceAreaDocument) XmlObject.Factory.parse(spdFile);
        this.currentModule = strSpdFilePath;
        this.saveFileName = strSpdFilePath;
        this.currentModuleType = 1;

        return xmlSpdDoc.getPackageSurfaceArea();
    }

    /**
     Open specificed Fpd file and read its content
     
     @param strFpdFilePath The input data of Fpd File Path
     
     **/
    private FrameworkPlatformDescriptionDocument.FrameworkPlatformDescription openFpdFile(String strFpdFilePath)
                                                                                                                throws IOException,
                                                                                                                XmlException,
                                                                                                                Exception {
        Log.log("Open Fpd", strFpdFilePath);
        File msaFile = new File(strFpdFilePath);
        xmlFpdDoc = (FrameworkPlatformDescriptionDocument) XmlObject.Factory.parse(msaFile);
        this.currentModule = strFpdFilePath;
        this.saveFileName = strFpdFilePath;
        this.currentModuleType = 1;

        return xmlFpdDoc.getFrameworkPlatformDescription();
    }

    /**
     Create an empty tree if no file is open
     
     **/
    private void makeEmptyTree() {
        //dmtnRoot = new IDefaultMutableTreeNode("No Msa/Mbd file opened", -1, -1);
        // Make root
        dmtnRoot = new IDefaultMutableTreeNode("WORKSPACE", -1, -1);

        // Make Module Description
        dmtnModuleDescription = new IDefaultMutableTreeNode("ModuleDescription", -1, -1);

        if (this.vModuleList.size() > 0) {
            for (int index = 0; index < this.vModuleList.size(); index++) {
                dmtnModuleDescription.add(new IDefaultMutableTreeNode(this.vModuleList.elementAt(index).getName(),
                                                                      IDefaultMutableTreeNode.MSA_HEADER, false,
                                                                      this.vModuleList.elementAt(index)));
            }
        }

        // Make Package Description
        dmtnPackageDescription = new IDefaultMutableTreeNode("PackageDescription", -1, -1);
        if (this.vPackageList.size() > 0) {
            for (int index = 0; index < this.vPackageList.size(); index++) {
                dmtnPackageDescription.add(new IDefaultMutableTreeNode(this.vPackageList.elementAt(index).getName(),
                                                                       IDefaultMutableTreeNode.SPD_HEADER, false,
                                                                       this.vPackageList.elementAt(index)));
            }
        }

        // Make Platform Description
        dmtnPlatformDescription = new IDefaultMutableTreeNode("PlatformDescription", -1, -1);
        if (this.vPlatformList.size() > 0) {
            for (int index = 0; index < this.vPlatformList.size(); index++) {
                dmtnPlatformDescription.add(new IDefaultMutableTreeNode(this.vPlatformList.elementAt(index).getName(),
                                                                        IDefaultMutableTreeNode.FPD_PLATFORMHEADER,
                                                                        false, this.vPlatformList.elementAt(index)));
            }
        }

        dmtnRoot.add(dmtnModuleDescription);
        dmtnRoot.add(dmtnPackageDescription);
        dmtnRoot.add(dmtnPlatformDescription);
        iTree = new ITree(dmtnRoot);
        iTree.addMouseListener(this);
        jScrollPaneTree.setViewportView(iTree);
    }

    /* (non-Javadoc)
     * @see java.awt.event.WindowListener#windowClosing(java.awt.event.WindowEvent)
     *
     * Override windowClosing to popup warning message to confirm quit
     * 
     */
    public void windowClosing(WindowEvent arg0) {
        this.exit();
    }

    /* (non-Javadoc)
     * @see java.awt.event.MouseListener#mouseClicked(java.awt.event.MouseEvent)
     * 
     * Override mouseClicked to check if need display popup menu
     * 
     */
    public void mouseClicked(MouseEvent arg0) {
        if (arg0.getButton() == MouseEvent.BUTTON1) {

        }
        if (arg0.getButton() == MouseEvent.BUTTON3) {
            jPopupMenu.show(arg0.getComponent(), arg0.getX(), arg0.getY());
        }
        if (arg0.getClickCount() == 2) {
            doubleClickModuleTreeNode();
        }
    }

    public void mouseEntered(MouseEvent arg0) {
        // TODO Auto-generated method stub
    }

    public void mouseExited(MouseEvent arg0) {
        // TODO Auto-generated method stub
    }

    public void mousePressed(MouseEvent arg0) {
        // TODO Auto-generated method stub
    }

    public void mouseReleased(MouseEvent arg0) {
        // TODO Auto-generated method stub
    }

    /**
     Init popup menu
     
     **/
    public void valueChanged(TreeSelectionEvent arg0) {
        int intOperation = iTree.getSelectOperation();
        if (intOperation == IDefaultMutableTreeNode.OPERATION_NULL) {
            setMenuItemAddEnabled(false);
            setMenuItemUpdateEnabled(false);
            setMenuItemDeleteEnabled(false);
        }
        if (intOperation == IDefaultMutableTreeNode.OPERATION_ADD) {
            setMenuItemAddEnabled(true);
            setMenuItemUpdateEnabled(false);
            setMenuItemDeleteEnabled(false);
        }
        if (intOperation == IDefaultMutableTreeNode.OPERATION_UPDATE) {
            setMenuItemAddEnabled(false);
            setMenuItemUpdateEnabled(true);
            setMenuItemDeleteEnabled(false);
        }
        if (intOperation == IDefaultMutableTreeNode.OPERATION_DELETE) {
            setMenuItemAddEnabled(false);
            setMenuItemUpdateEnabled(false);
            setMenuItemDeleteEnabled(true);
        }
        if (intOperation == IDefaultMutableTreeNode.OPERATION_ADD_UPDATE) {
            setMenuItemAddEnabled(true);
            setMenuItemUpdateEnabled(true);
            setMenuItemDeleteEnabled(false);
        }
        if (intOperation == IDefaultMutableTreeNode.OPERATION_ADD_DELETE) {
            setMenuItemAddEnabled(true);
            setMenuItemUpdateEnabled(false);
            setMenuItemDeleteEnabled(true);
        }
        if (intOperation == IDefaultMutableTreeNode.OPERATION_UPDATE_DELETE) {
            setMenuItemAddEnabled(false);
            setMenuItemUpdateEnabled(true);
            setMenuItemDeleteEnabled(true);
        }
        if (intOperation == IDefaultMutableTreeNode.OPERATION_ADD_UPDATE_DELETE) {
            setMenuItemAddEnabled(true);
            setMenuItemUpdateEnabled(true);
            setMenuItemDeleteEnabled(true);
        }
    }

    /**
     Enable/Disable add menu item
     
     **/
    private void setMenuItemAddEnabled(boolean isEnable) {
        jMenuItemPopupAdd.setEnabled(isEnable);
    }

    /**
     Enable/Disable update menu item
     
     **/
    private void setMenuItemUpdateEnabled(boolean isEnable) {
        jMenuItemPopupUpdate.setEnabled(isEnable);
        //jMenuItemEditUpdate.setEnabled(isEnable);
    }

    /**
     Enable/Disable delete menu item
     
     **/
    private void setMenuItemDeleteEnabled(boolean isEnable) {
        jMenuItemPopupDelete.setEnabled(isEnable);
        jMenuItemEditDelete.setEnabled(isEnable);
    }

    /**
     Close current open module

     **/
    private void closeCurrentModule() {
        cleanAllXml();

        //reloadTreeAndTable(FrameworkWizardUI.CLOSED);
        setMenuItemAddEnabled(false);
        setMenuItemUpdateEnabled(false);
        setMenuItemDeleteEnabled(false);
        //makeEmptyTree();
        cleanDesktopPane();
    }

    /**
     Remove all Internal Frame of Module Desktop Pane
     
     **/
    private void cleanDesktopPaneModule() {
        if (jDesktopPaneModule != null) {
            JInternalFrame[] iif = this.jDesktopPaneModule.getAllFrames();
            for (int index = 0; index < iif.length; index++) {
                iif[index].dispose();
            }
        }
    }

    /**
     Remove all Internal Frame of package Desktop Pane
     
     **/
    private void cleanDesktopPanePackage() {
        if (jDesktopPanePlatform != null) {
            JInternalFrame[] iif = this.jDesktopPanePackage.getAllFrames();
            for (int index = 0; index < iif.length; index++) {
                iif[index].dispose();
            }
        }
    }

    /**
     Remove all Internal Frame of platform Desktop Pane
     
     **/
    private void cleanDesktopPanePlatform() {
        if (jDesktopPanePlatform != null) {
            JInternalFrame[] iif = this.jDesktopPanePlatform.getAllFrames();
            for (int index = 0; index < iif.length; index++) {
                iif[index].dispose();
            }
        }
    }

    /**
     Remove all Internal Frame of all Desktop Panes
     
     **/
    private void cleanDesktopPane() {
        cleanDesktopPaneModule();
        cleanDesktopPanePackage();
        cleanDesktopPanePlatform();
    }

    /**
     Set all xml document null
     
     **/
    private void cleanAllXml() {
        this.currentModule = "";
        this.saveFileName = "";
        this.currentModuleType = 0;

        xmlMsaDoc = null;
    }

    /**
     Save current module
     Call relevant function via different file types
     
     **/
    private void saveCurrentModule() {
        if (this.saveFileName == "") {
            openFile(2, this.currentModuleType);
        }
        if (this.saveFileName == "") {
            this.saveFileName = this.currentModule;
            return;
        } else {
            switch (this.currentModuleType) {
            case 1:
                saveMsa();
                break;
            }

        }
        isSaved = true;
    }

    /**
     Save current module as
     
     **/
    private void saveAsCurrentModule() {
        this.saveFileName = "";
        saveCurrentModule();
    }

    /**
     Save file as msa
     
     **/
    private void saveMsa() {
        //File f = new File(this.saveFileName);
        File f = new File("C://a.msa");
        ModuleSurfaceAreaDocument msaDoc = ModuleSurfaceAreaDocument.Factory.newInstance();
        ModuleSurfaceAreaDocument.ModuleSurfaceArea msa = ModuleSurfaceAreaDocument.ModuleSurfaceArea.Factory
                                                                                                             .newInstance();
        msa = lstOpeningFile.getVOpeningModuleList().elementAt(0).getXmlMsa();

        //
        //Init namespace
        //
        XmlCursor cursor = msa.newCursor();
        String uri = "http://www.TianoCore.org/2006/Edk2.0";
        cursor.push();
        cursor.toNextToken();
        cursor.insertNamespace("", uri);
        cursor.insertNamespace("xsi", "http://www.w3.org/2001/XMLSchema-instance");
        cursor.pop();

        //
        //Config file format
        //
        XmlOptions options = new XmlOptions();
        options.setCharacterEncoding("UTF-8");
        options.setSavePrettyPrint();
        options.setSavePrettyPrintIndent(2);

        //
        //Create finial doc
        //
        msaDoc.addNewModuleSurfaceArea();
        msaDoc.setModuleSurfaceArea((ModuleSurfaceAreaDocument.ModuleSurfaceArea) cursor.getObject());
        try {
            //
            //Save the file
            //
            msaDoc.save(f, options);
            this.currentModule = this.saveFileName;
        } catch (Exception e) {
            Log.err("Save Msa", e.getMessage());
        }
    }

    /**
     Reflash the tree via current value of xml documents.
     
     @param intMode The input data of current operation type
     
     **/
    private void reloadTreeAndTable(int intMode) {
        //makeTree();
        if (intMode == FrameworkWizardUI.OPENED) {
            this.jMenuItemFileClose.setEnabled(true);
            this.jMenuItemFileSaveAs.setEnabled(true);
            //this.jMenuEditAdd.setEnabled(true);
            this.setTitle(windowTitle + "- [" + this.currentModule + "]");
            this.jButtonOk.setEnabled(false);
            this.jButtonCancel.setEnabled(false);
        }
        if (intMode == FrameworkWizardUI.CLOSED) {
            this.jMenuItemFileClose.setEnabled(false);
            this.jMenuItemFileSave.setEnabled(false);
            this.jMenuItemFileSaveAs.setEnabled(false);
            this.setTitle(windowTitle + "- [" + ws.getCurrentWorkspace() + "]");
            this.setButtonEnable(false);
        }
        if (intMode == FrameworkWizardUI.NEW_WITHOUT_CHANGE) {

        }

        if (intMode == FrameworkWizardUI.NEW_WITH_CHANGE) {
            this.jMenuItemFileClose.setEnabled(true);
            this.jMenuItemFileSave.setEnabled(true);
            this.jMenuItemFileSaveAs.setEnabled(true);
            //this.jMenuEditAdd.setEnabled(true);
            setButtonEnable(false);
        }
        if (intMode == FrameworkWizardUI.UPDATE_WITHOUT_CHANGE) {

        }
        if (intMode == FrameworkWizardUI.UPDATE_WITH_CHANGE) {
            this.jMenuItemFileClose.setEnabled(true);
            this.jMenuItemFileSave.setEnabled(true);
            this.jMenuItemFileSaveAs.setEnabled(true);
        }
        if (intMode == FrameworkWizardUI.SAVE_WITHOUT_CHANGE) {
            this.jMenuItemFileClose.setEnabled(true);
            this.jMenuItemFileSave.setEnabled(true);
            this.jMenuItemFileSaveAs.setEnabled(true);
            this.jButtonOk.setEnabled(false);
            this.jButtonCancel.setEnabled(false);
        }
        if (intMode == FrameworkWizardUI.SAVE_WITH_CHANGE) {
            this.jMenuItemFileClose.setEnabled(true);
            this.jMenuItemFileSave.setEnabled(false);
            this.jMenuItemFileSaveAs.setEnabled(true);
            //this.jMenuItemEditUpdate.setEnabled(false);
            this.jMenuItemEditDelete.setEnabled(false);
            this.setTitle(windowTitle + "- [" + this.currentModule + "]");
            this.jButtonOk.setEnabled(false);
            this.jButtonCancel.setEnabled(false);
        }
    }

    /**
     Enable/Disable button Ok and Cancel
     
     @param isEnabled The input data to indicate if button is enabled or not
     
     **/
    private void setButtonEnable(boolean isEnabled) {
        this.jButtonCancel.setEnabled(isEnabled);
        this.jButtonOk.setEnabled(isEnabled);
    }

    public void componentHidden(ComponentEvent arg0) {
        // TODO Auto-generated method stub

    }

    public void componentMoved(ComponentEvent arg0) {
        // TODO Auto-generated method stub

    }

    public void componentResized(ComponentEvent arg0) {
        this.jSplitPane.setSize(this.getWidth() - DataType.MAIN_FRAME_WIDTH_SPACING,
                                this.getHeight() - DataType.MAIN_FRAME_HEIGHT_SPACING);
        this.jSplitPane.validate();
        resizeDesktopPanel();
    }

    public void componentShown(ComponentEvent arg0) {
        // TODO Auto-generated method stub

    }

    /**
     Resize JDesktopPanel
     
     */
    private void resizeDesktopPanel() {
        resizeDesktopPanel(this.jDesktopPaneModule);
        resizeDesktopPanel(this.jDesktopPanePackage);
        resizeDesktopPanel(this.jDesktopPanePlatform);
    }

    /**
     Resize JDesktopPanel
     
     */
    private void resizeDesktopPanel(JDesktopPane jdk) {
        JInternalFrame[] iif = jdk.getAllFrames();
        for (int index = 0; index < iif.length; index++) {
            iif[index].setSize(jdk.getWidth(), jdk.getHeight());
        }
    }

    public void stateChanged(ChangeEvent arg0) {
        // TODO Auto-generated method stub

    }

    private void getCompontentsFromFrameworkDatabase() {
        this.vModuleList = ws.getAllModules();
        this.vPackageList = ws.getAllPackages();
        this.vPlatformList = ws.getAllPlatforms();
    }

    private void insertModuleTreeNode(Identification id) {
        iTree.addNode(new IDefaultMutableTreeNode("Library Class Definitions",
                                                  IDefaultMutableTreeNode.MSA_LIBRARYCLASSDEFINITIONS, true, id));
        iTree.addNode(new IDefaultMutableTreeNode("Package Dependencies",
                                                  IDefaultMutableTreeNode.MSA_PACKAGEDEPENDENCIES, true, id));
        iTree.addNode(new IDefaultMutableTreeNode("Source Files", IDefaultMutableTreeNode.MSA_SOURCEFILES, true, id));
        iTree.addNode(new IDefaultMutableTreeNode("Protocols", IDefaultMutableTreeNode.MSA_PROTOCOLS, true, id));
        iTree.addNode(new IDefaultMutableTreeNode("Events", IDefaultMutableTreeNode.MSA_EVENTS, true, id));
        iTree.addNode(new IDefaultMutableTreeNode("Hobs", IDefaultMutableTreeNode.MSA_HOBS, true, id));
        iTree.addNode(new IDefaultMutableTreeNode("Ppis", IDefaultMutableTreeNode.MSA_PPIS, true, id));
        iTree.addNode(new IDefaultMutableTreeNode("Variables", IDefaultMutableTreeNode.MSA_VARIABLES, true, id));
        iTree.addNode(new IDefaultMutableTreeNode("Boot Modes", IDefaultMutableTreeNode.MSA_BOOTMODES, true, id));
        iTree.addNode(new IDefaultMutableTreeNode("System Tables", IDefaultMutableTreeNode.MSA_SYSTEMTABLES, true, id));
        iTree.addNode(new IDefaultMutableTreeNode("Data Hubs", IDefaultMutableTreeNode.MSA_DATAHUBS, true, id));
        iTree.addNode(new IDefaultMutableTreeNode("Formats", IDefaultMutableTreeNode.MSA_FORMSETS, true, id));
        iTree.addNode(new IDefaultMutableTreeNode("Guids", IDefaultMutableTreeNode.MSA_GUIDS, true, id));
        iTree.addNode(new IDefaultMutableTreeNode("Externs", IDefaultMutableTreeNode.MSA_EXTERNS, true, id));
        iTree.addNode(new IDefaultMutableTreeNode("PCDs", IDefaultMutableTreeNode.MSA_PCDS, true, id));
    }

    private void insertPackageTreeNode(Identification id) {
        iTree.addNode(new IDefaultMutableTreeNode("Library Class Declarations",
                                                  IDefaultMutableTreeNode.SPD_LIBRARYCLASSDECLARATIONS, true, id));
        iTree.addNode(new IDefaultMutableTreeNode("Msa Files", IDefaultMutableTreeNode.SPD_MSAFILES, false, id));
        iTree.addNode(new IDefaultMutableTreeNode("Package Headers", IDefaultMutableTreeNode.SPD_PACKAGEHEADERS, true,
                                                  id));
        iTree.addNode(new IDefaultMutableTreeNode("Guid Declarations", IDefaultMutableTreeNode.SPD_GUIDDECLARATIONS,
                                                  true, id));
        iTree.addNode(new IDefaultMutableTreeNode("Protocol Declarations",
                                                  IDefaultMutableTreeNode.SPD_PROTOCOLDECLARATIONS, true, id));
        iTree.addNode(new IDefaultMutableTreeNode("Ppi Declarations", IDefaultMutableTreeNode.SPD_PPIDECLARATIONS,
                                                  true, id));
        iTree.addNode(new IDefaultMutableTreeNode("Pcd Declarations", IDefaultMutableTreeNode.SPD_PCDDECLARATIONS,
                                                  true, id));
    }

    private void insertPlatformTreeNode(Identification id) {
        iTree.addNode(new IDefaultMutableTreeNode("Flash", IDefaultMutableTreeNode.FPD_FLASH, true, id));
        iTree.addNode(new IDefaultMutableTreeNode("Framework Modules", IDefaultMutableTreeNode.FPD_FRAMEWORKMODULES,
                                                  true, id));
        iTree.addNode(new IDefaultMutableTreeNode("Pcd Dynamic Build Declarations",
                                                  IDefaultMutableTreeNode.FPD_PCDDYNAMICBUILDDECLARATIONS, true, id));
        iTree.addNode(new IDefaultMutableTreeNode("Build Options", IDefaultMutableTreeNode.FPD_BUILDOPTIONS, true, id));
    }

    /**
     Operate when double click a tree node
     
     **/
    private void doubleClickModuleTreeNode() {
        Identification id = iTree.getSelectNode().getId();
        int intCategory = iTree.getSelectCategory();
        ModuleSurfaceAreaDocument.ModuleSurfaceArea msa = null;
        PackageSurfaceAreaDocument.PackageSurfaceArea spd = null;
        FrameworkPlatformDescriptionDocument.FrameworkPlatformDescription fpd = null;

        //          
        // If the node is not opened yet
        // Insert top level elements first
        //
        if (intCategory == IDefaultMutableTreeNode.MSA_HEADER || intCategory == IDefaultMutableTreeNode.SPD_MSAFILES) {
            if (!lstOpeningFile.existsModule(id)) {
                String strMsaFilePath = "";
                if (intCategory == IDefaultMutableTreeNode.MSA_HEADER) {
                    strMsaFilePath = iTree.getSelectNode().getId().getName();
                }
                if (intCategory == IDefaultMutableTreeNode.SPD_MSAFILES) {
                    strMsaFilePath = iTree.getSelectNode().getId().getName();
                }
                try {
                    msa = openMsaFile(strMsaFilePath);
                } catch (IOException e) {
                    Log.err("Open Msa " + strMsaFilePath, e.getMessage());
                    return;
                } catch (XmlException e) {
                    Log.err("Open Msa " + strMsaFilePath, e.getMessage());
                    return;
                } catch (Exception e) {
                    Log.err("Open Msa " + strMsaFilePath, "Invalid file type");
                    return;
                }
                lstOpeningFile.insertToOpeningModuleList(id, msa);
                insertModuleTreeNode(id);
                iTree.getSelectNode().setOpening(true);
            }
        }
        if (iTree.getSelectCategory() == IDefaultMutableTreeNode.SPD_HEADER) {
            if (!lstOpeningFile.existsPackage(id)) {
                String strSpdFilePath = iTree.getSelectNode().getId().getPath();
                try {
                    spd = openSpdFile(strSpdFilePath);
                } catch (IOException e) {
                    Log.err("Open Spd " + strSpdFilePath, e.getMessage());
                    return;
                } catch (XmlException e) {
                    Log.err("Open Spd " + strSpdFilePath, e.getMessage());
                    return;
                } catch (Exception e) {
                    Log.err("Open Spd " + strSpdFilePath, "Invalid file type");
                    return;
                }
                lstOpeningFile.insertToOpeningPackageList(id, spd);
                insertPackageTreeNode(id);
                iTree.getSelectNode().setOpening(true);
            }
        }
        if (iTree.getSelectCategory() == IDefaultMutableTreeNode.FPD_PLATFORMHEADER) {
            if (!lstOpeningFile.existsPlatform(id)) {
                String strFpdFilePath = iTree.getSelectNode().getId().getPath();
                try {
                    fpd = openFpdFile(strFpdFilePath);
                } catch (IOException e) {
                    Log.err("Open Fpd " + strFpdFilePath, e.getMessage());
                    return;
                } catch (XmlException e) {
                    Log.err("Open Fpd " + strFpdFilePath, e.getMessage());
                    return;
                } catch (Exception e) {
                    Log.err("Open Fpd " + strFpdFilePath, "Invalid file type");
                    return;
                }
                lstOpeningFile.insertToOpeningPlatformList(id, fpd);
                insertPlatformTreeNode(id);
                iTree.getSelectNode().setOpening(true);
            }
        }
        if (intCategory >= IDefaultMutableTreeNode.MSA_HEADER && intCategory < IDefaultMutableTreeNode.SPD_HEADER) {
            showModuleElement(intCategory, lstOpeningFile.getModuleSurfaceAreaFromId(id));
        }
        if (intCategory >= IDefaultMutableTreeNode.SPD_HEADER
            && intCategory < IDefaultMutableTreeNode.FPD_PLATFORMHEADER) {
            showPackageElement(intCategory, lstOpeningFile.getPackageSurfaceAreaFromId(id));
        }
        if (intCategory >= IDefaultMutableTreeNode.FPD_PLATFORMHEADER) {
            showPlatformElement(intCategory, lstOpeningFile.getFrameworkPlatformDescriptionFromId(id));
        }
    }

    /**
     Show content of editor panel via selected element
     
     @param elementType
     @param fpd
     
     */
    private void showPlatformElement(int elementType,
                                     FrameworkPlatformDescriptionDocument.FrameworkPlatformDescription fpd) {
        this.cleanDesktopPanePlatform();
        switch (elementType) {
        case IDefaultMutableTreeNode.FPD_PLATFORMHEADER:
            break;
        case IDefaultMutableTreeNode.FPD_FLASH:
            break;
        case IDefaultMutableTreeNode.FPD_FRAMEWORKMODULES:
            break;
        case IDefaultMutableTreeNode.FPD_PCDDYNAMICBUILDDECLARATIONS:
            break;
        case IDefaultMutableTreeNode.FPD_BUILDOPTIONS:
            break;
        }
        this.jTabbedPaneEditor.setSelectedIndex(2);
        resizeDesktopPanel();
    }

    /**
     Show content of editor panel via selected element
     
     @param elementType
     @param spd
     
     */
    private void showPackageElement(int elementType, PackageSurfaceAreaDocument.PackageSurfaceArea spd) {
        this.cleanDesktopPanePackage();
        switch (elementType) {
        case IDefaultMutableTreeNode.SPD_HEADER:
            SpdHeader frmSpdHeader = new SpdHeader();
            getJDesktopPanePackage().add(frmSpdHeader, 1);
            break;
        case IDefaultMutableTreeNode.SPD_LIBRARYCLASSDECLARATIONS:
            SpdLibClassDecls frmSlcd = new SpdLibClassDecls();
            getJDesktopPanePackage().add(frmSlcd, 1);
            break;
        case IDefaultMutableTreeNode.SPD_MSAFILES:
            SpdMsaFiles frmSmf = new SpdMsaFiles();
            getJDesktopPanePackage().add(frmSmf, 1);
            break;
        case IDefaultMutableTreeNode.SPD_PACKAGEHEADERS:
            SpdPackageHeaders frmSph = new SpdPackageHeaders();
            getJDesktopPanePackage().add(frmSph, 1);
            break;
        case IDefaultMutableTreeNode.SPD_GUIDDECLARATIONS:
            SpdGuidDecls frmSgd = new SpdGuidDecls();
            getJDesktopPanePackage().add(frmSgd, 1);
            break;
        case IDefaultMutableTreeNode.SPD_PROTOCOLDECLARATIONS:
            SpdProtocolDecls frmSprod = new SpdProtocolDecls();
            getJDesktopPanePackage().add(frmSprod, 1);
            break;
        case IDefaultMutableTreeNode.SPD_PPIDECLARATIONS:
            SpdPpiDecls frmSppid = new SpdPpiDecls();
            getJDesktopPanePackage().add(frmSppid, 1);
            break;
        case IDefaultMutableTreeNode.SPD_PCDDECLARATIONS:
            SpdPcdDefs frmSpcdd = new SpdPcdDefs();
            getJDesktopPanePackage().add(frmSpcdd, 1);
            break;
        }
        this.jTabbedPaneEditor.setSelectedIndex(1);
        resizeDesktopPanel();
    }

    /**
     Show content of editor panel via selected element
     
     @param elementType
     @param msa
     
     */
    private void showModuleElement(int elementType, ModuleSurfaceAreaDocument.ModuleSurfaceArea msa) {
        this.cleanDesktopPaneModule();
        switch (elementType) {
        case IDefaultMutableTreeNode.MSA_HEADER:
            MsaHeader frmMsaHeader = new MsaHeader(msa);
            getJDesktopPaneModule().add(frmMsaHeader, 1);
            break;
        case IDefaultMutableTreeNode.MSA_LIBRARYCLASSDEFINITIONS:
            ModuleLibraryClassDefinitions frmMlcd = new ModuleLibraryClassDefinitions(msa);
            getJDesktopPaneModule().add(frmMlcd, 1);
            break;
        case IDefaultMutableTreeNode.MSA_PACKAGEDEPENDENCIES:
            ModulePackageDependencies frmMpd = new ModulePackageDependencies();
            getJDesktopPaneModule().add(frmMpd, 1);
            break;
        case IDefaultMutableTreeNode.MSA_SOURCEFILES:
            ModuleSourceFiles frmMsf = new ModuleSourceFiles();
            getJDesktopPaneModule().add(frmMsf, 1);
            break;
        case IDefaultMutableTreeNode.MSA_PROTOCOLS:
            ModuleProtocols frmMp = new ModuleProtocols();
            getJDesktopPaneModule().add(frmMp, 1);
            break;
        case IDefaultMutableTreeNode.MSA_EVENTS:
            ModuleEvents frmMe = new ModuleEvents();
            getJDesktopPaneModule().add(frmMe, 1);
            break;
        case IDefaultMutableTreeNode.MSA_HOBS:
            ModuleHobs frmMh = new ModuleHobs();
            getJDesktopPaneModule().add(frmMh, 1);
            break;
        case IDefaultMutableTreeNode.MSA_PPIS:
            ModulePpis frmMpp = new ModulePpis();
            getJDesktopPaneModule().add(frmMpp, 1);
            break;
        case IDefaultMutableTreeNode.MSA_VARIABLES:
            ModuleVariables frmMv = new ModuleVariables();
            getJDesktopPaneModule().add(frmMv, 1);
            break;
        case IDefaultMutableTreeNode.MSA_BOOTMODES:
            ModuleBootModes frmMbm = new ModuleBootModes();
            getJDesktopPaneModule().add(frmMbm, 1);
            break;
        case IDefaultMutableTreeNode.MSA_SYSTEMTABLES:
            ModuleSystemTables frmMst = new ModuleSystemTables();
            getJDesktopPaneModule().add(frmMst, 1);
            break;
        case IDefaultMutableTreeNode.MSA_DATAHUBS:
            ModuleDataHubs frmMdh = new ModuleDataHubs();
            getJDesktopPaneModule().add(frmMdh, 1);
            break;
        case IDefaultMutableTreeNode.MSA_FORMSETS:
            ModuleFormsets frmMf = new ModuleFormsets();
            getJDesktopPaneModule().add(frmMf, 1);
            break;
        case IDefaultMutableTreeNode.MSA_GUIDS:
            ModuleGuids frmGuid = new ModuleGuids();
            getJDesktopPaneModule().add(frmGuid, 1);
            break;
        case IDefaultMutableTreeNode.MSA_EXTERNS:
            ModuleExterns frmMex = new ModuleExterns();
            getJDesktopPaneModule().add(frmMex, 1);
            break;
        case IDefaultMutableTreeNode.MSA_PCDS:
            ModulePCDs frmPcd = new ModulePCDs();
            getJDesktopPaneModule().add(frmPcd, 1);
            break;
        }
        this.jTabbedPaneEditor.setSelectedIndex(0);
        resizeDesktopPanel();
    }

    //
    // Define operations of menu
    //
    private void fileNew() {

    }

    private void open() {

    }

    private void close() {

    }

    /**
     Close all opening files and clean all showing internal frame
     
     **/
    private void closeAll() {
        this.cleanDesktopPane();
        this.makeEmptyTree();
        lstOpeningFile.removeAll();
    }

    /**
     Save currentModule when press button OK
     
     **/
    private void save() {
        // TODO sdfasd
    }

    private void saveAs() {

    }

    private void saveAll() {

    }

    /**
     To save changed items before exit.
     
     **/
    private void exit() {
        if (isSaved == false) {
            //dsfsdfsdf
        }
        this.dispose();
        System.exit(0);
    }
}
