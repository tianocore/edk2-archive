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
import org.tianocore.BootModesDocument;
import org.tianocore.DataHubsDocument;
import org.tianocore.EventsDocument;
import org.tianocore.ExternsDocument;
import org.tianocore.FormsetsDocument;
import org.tianocore.GuidsDocument;
import org.tianocore.HobsDocument;
import org.tianocore.LibraryClassDefinitionsDocument;
import org.tianocore.ModuleSurfaceAreaDocument;
import org.tianocore.MsaHeaderDocument;
import org.tianocore.PCDsDocument;
import org.tianocore.PPIsDocument;
import org.tianocore.ProtocolsDocument;
import org.tianocore.SourceFilesDocument;
import org.tianocore.SystemTablesDocument;
import org.tianocore.VariablesDocument;
import org.tianocore.frameworkwizard.common.DataType;
import org.tianocore.frameworkwizard.common.IFileFilter;
import org.tianocore.frameworkwizard.common.Log;
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
import org.tianocore.frameworkwizard.module.ui.ModulePpis;
import org.tianocore.frameworkwizard.module.ui.ModuleProtocols;
import org.tianocore.frameworkwizard.module.ui.ModuleSourceFiles;
import org.tianocore.frameworkwizard.module.ui.ModuleSystemTables;
import org.tianocore.frameworkwizard.module.ui.ModuleVariables;
import org.tianocore.frameworkwizard.module.ui.MsaHeader;
import org.tianocore.frameworkwizard.packaging.PackageIdentification;
import org.tianocore.frameworkwizard.platform.PlatformIdentification;
import org.tianocore.frameworkwizard.workspace.Workspace;
import javax.swing.JCheckBoxMenuItem;

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

    private Vector<ModuleIdentification> vOpeningModuleList = new Vector<ModuleIdentification>();

    private Vector<PackageIdentification> vOpeningPackageList = new Vector<PackageIdentification>();

    private Vector<PlatformIdentification> vOpeningPlatformList = new Vector<PlatformIdentification>();

    ///
    ///  0 - reserved; 1 - msa; 2 - mbd; 3 - lmsa; 4 - lmbd;
    ///
    private int currentModuleType = 0;

    private int currentNodeType = -1;

    private String windowTitle = "FrameworkWizard 1.0 ";

    private ModuleSurfaceAreaDocument xmlMsaDoc = null;

    private MsaHeaderDocument.MsaHeader xmlmh = null;

    private LibraryClassDefinitionsDocument.LibraryClassDefinitions xmllcd = null;

    private SourceFilesDocument.SourceFiles xmlsf = null;

    private ProtocolsDocument.Protocols xmlpl = null;

    private EventsDocument.Events xmlen = null;

    private HobsDocument.Hobs xmlhob = null;

    private PPIsDocument.PPIs xmlppi = null;

    private VariablesDocument.Variables xmlvb = null;

    private BootModesDocument.BootModes xmlbm = null;

    private SystemTablesDocument.SystemTables xmlst = null;

    private DataHubsDocument.DataHubs xmldh = null;

    private FormsetsDocument.Formsets xmlfs = null;

    private GuidsDocument.Guids xmlgu = null;

    private ExternsDocument.Externs xmlet = null;

    private PCDsDocument.PCDs xmlpcd = null;

    private IDefaultMutableTreeNode dmtnRoot = null;

    private IDefaultMutableTreeNode dmtnModuleDescription = null;

    private IDefaultMutableTreeNode dmtnPackageDescription = null;

    private IDefaultMutableTreeNode dmtnPlatformDescription = null;

    private JPanel jContentPane = null;

    private JMenuBar jMenuBar = null;

    private JMenu jMenuFile = null;

    private JMenuItem jMenuItemFileNew = null;

    private JMenuItem jMenuItemModuleNewModule = null;

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

    private static final int ADD = 1;

    private static final int UPDATE = 2;

    //private static final int DELETE = 3;

    private static final int VIEW = 4;

    private MsaHeader msa = null;

    private ModuleLibraryClassDefinitions mlcd = null;

    private ModuleSourceFiles msf = null;

    private ModuleProtocols mp = null;

    private ModuleEvents mev = null;

    private ModuleHobs mh = null;

    private ModulePpis mpp = null;

    private ModuleVariables mv = null;

    private ModuleBootModes mbm = null;

    private ModuleSystemTables mst = null;

    private ModuleDataHubs mdh = null;

    private ModuleFormsets mf = null;

    private ModuleGuids mg = null;

    private ModuleExterns met = null;

    private ModulePCDs mpcd = null;

    //private JMenuItem jMenuItemModuleOpenModule = null;

    //private JMenu jMenuFileOpen = null;

    private JMenuItem jMenuItemModuleOpenModuleBuildDescription = null;

    private JMenuItem jMenuItemModuleOpenLibraryModule = null;

    private JMenuItem jMenuItemModuleOpenLibraryModuleBuildDescription = null;

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
            jTabbedPaneEditor.addTab("Package", null, getJDesktopPanePlatform(), null);
            jTabbedPaneEditor.addTab("Platform", null, getJDesktopPanePackage(), null);
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
     This method initializes jMenuItemModuleNewModule 
     
     @return javax.swing.JMenuItem jMenuItemModuleNewModule
     
     **/
    //    private JMenuItem getJMenuItemModuleNewModule() {
    //        if (jMenuItemModuleNewModule == null) {
    //            jMenuItemModuleNewModule = new JMenuItem();
    //            jMenuItemModuleNewModule.setText("Module (.msa)");
    //            jMenuItemModuleNewModule.addActionListener(this);
    //        }
    //        return jMenuItemModuleNewModule;
    //    }
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
            //jMenuItemFileNew.add(getJMenuItemModuleNewModule());
            //jMenuItemFileNew.add(getJMenuItemModuleNewModuleBuildDescription());
            //jMenuItemFileNew.add(getJMenuItemModuleNewLibraryModule());
            //jMenuItemFileNew.add(getJMenuItemModuleNewLibraryModuleBuildDescription());
        }
        return jMenuItemFileNew;
    }

    /**
     This method initializes jMenuItemModuleOpenModule 
     
     @return javax.swing.JMenuItem jMenuItemModuleOpenModule
     
     **/
    //    private JMenuItem getJMenuItemModuleOpenModule() {
    //        if (jMenuItemModuleOpenModule == null) {
    //            jMenuItemModuleOpenModule = new JMenuItem();
    //            jMenuItemModuleOpenModule.setText("Module (.msa)");
    //            jMenuItemModuleOpenModule.addActionListener(this);
    //        }
    //        return jMenuItemModuleOpenModule;
    //    }
    /**
     This method initializes jMenuItemFileNewModuleBuildDescription 
     
     @return javax.swing.JMenuItem jMenuItemModuleNewModuleBuildDescription
     
     **/
    //    private JMenuItem getJMenuItemModuleNewModuleBuildDescription() {
    //        if (jMenuItemModuleNewModuleBuildDescription == null) {
    //            jMenuItemModuleNewModuleBuildDescription = new JMenuItem();
    //            jMenuItemModuleNewModuleBuildDescription.setText("Module Build Description (.mbd)");
    //            jMenuItemModuleNewModuleBuildDescription.addActionListener(this);
    //        }
    //        return jMenuItemModuleNewModuleBuildDescription;
    //    }
    /**
     This method initializes jMenuItemFileNewLibraryModule 
     
     @return javax.swing.JMenuItem jMenuItemModuleNewLibraryModule
     
     **/
    //    private JMenuItem getJMenuItemModuleNewLibraryModule() {
    //        if (jMenuItemModuleNewLibraryModule == null) {
    //            jMenuItemModuleNewLibraryModule = new JMenuItem();
    //            jMenuItemModuleNewLibraryModule.setText("Library Module (*.msa)");
    //            jMenuItemModuleNewLibraryModule.addActionListener(this);
    //        }
    //        return jMenuItemModuleNewLibraryModule;
    //    }
    /**
     This method initializes jMenuItemFileNewLibraryModuleBuildDescription 
     
     @return javax.swing.JMenuItem jMenuItemModuleNewLibraryModuleBuildDescription
     
     **/
    //    private JMenuItem getJMenuItemModuleNewLibraryModuleBuildDescription() {
    //        if (jMenuItemModuleNewLibraryModuleBuildDescription == null) {
    //            jMenuItemModuleNewLibraryModuleBuildDescription = new JMenuItem();
    //            jMenuItemModuleNewLibraryModuleBuildDescription.setText("Library Module Build Description (.mbd)");
    //            jMenuItemModuleNewLibraryModuleBuildDescription.addActionListener(this);
    //        }
    //        return jMenuItemModuleNewLibraryModuleBuildDescription;
    //    }
    /**
     This method initializes jMenuOpen 
     
     @return javax.swing.JMenu jMenuModuleOpen
     
     **/
    //    private JMenu getJMenuFileOpen() {
    //        if (jMenuFileOpen == null) {
    //            jMenuFileOpen = new JMenu();
    //            jMenuFileOpen.setText("Open...");
    //            jMenuFileOpen.setMnemonic('O');
    //            jMenuFileOpen.add(getJMenuItemModuleOpenModule());
    //            jMenuFileOpen.add(getJMenuItemModuleOpenModuleBuildDescription());
    //            jMenuFileOpen.add(getJMenuItemModuleOpenLibraryModule());
    //            jMenuFileOpen.add(getJMenuItemModuleOpenLibraryModuleBuildDescription());
    //        }
    //        return jMenuFileOpen;
    //    }
    /**
     This method initializes jMenuItemFileOpenModuleBuildDescription 
     
     @return javax.swing.JMenuItem jMenuItemModuleOpenModuleBuildDescription
     
     **/
    //    private JMenuItem getJMenuItemModuleOpenModuleBuildDescription() {
    //        if (jMenuItemModuleOpenModuleBuildDescription == null) {
    //            jMenuItemModuleOpenModuleBuildDescription = new JMenuItem();
    //            jMenuItemModuleOpenModuleBuildDescription.setText("Module Build Description (.mbd)");
    //            jMenuItemModuleOpenModuleBuildDescription.addActionListener(this);
    //        }
    //        return jMenuItemModuleOpenModuleBuildDescription;
    //    }
    /**
     This method initializes jMenuItemFileOpenLibraryModule 
     
     @return javax.swing.JMenuItem jMenuItemModuleOpenLibraryModule
     
     **/
    //    private JMenuItem getJMenuItemModuleOpenLibraryModule() {
    //        if (jMenuItemModuleOpenLibraryModule == null) {
    //            jMenuItemModuleOpenLibraryModule = new JMenuItem();
    //            jMenuItemModuleOpenLibraryModule.setText("Library Module (.msa)");
    //            jMenuItemModuleOpenLibraryModule.addActionListener(this);
    //        }
    //        return jMenuItemModuleOpenLibraryModule;
    //    }
    /**
     This method initializes jMenuItemFileOpenLibraryModuleBuildDescription 
     
     @return javax.swing.JMenuItem jMenuItemModuleOpenLibraryModuleBuildDescription
     
     **/
    //    private JMenuItem getJMenuItemModuleOpenLibraryModuleBuildDescription() {
    //        if (jMenuItemModuleOpenLibraryModuleBuildDescription == null) {
    //            jMenuItemModuleOpenLibraryModuleBuildDescription = new JMenuItem();
    //            jMenuItemModuleOpenLibraryModuleBuildDescription.setText("Library Module Build Description (.mbd)");
    //            jMenuItemModuleOpenLibraryModuleBuildDescription.addActionListener(this);
    //        }
    //        return jMenuItemModuleOpenLibraryModuleBuildDescription;
    //    }
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
            jMenuItemFileSave.setEnabled(false);
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

    /**
     * This method initializes jToolBarWindow	
     * 	
     * @return javax.swing.JToolBar	
     */
    //	private JToolBar getJToolBarWindow() {
    //		if (jToolBarWindow == null) {
    //			jToolBarWindow = new JToolBar();
    //			jToolBarWindow.setFloatable(false);
    //		}
    //		return jToolBarWindow;
    //	}
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
            //jContentPane.add(getJToolBarFile(), null);
            //jContentPane.add(getJToolBarEdit(), null);
            //jContentPane.add(getJToolBarWindow(), null);
            jContentPane.add(getJPanelOperation(), null);
            jContentPane.add(getJSplitPane(), null);
            //jContentPane.add(getjDesktopPaneModule(), null);
            //jContentPane.add(getJScrollPaneTree(), null);
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
        // Open relevant frame via clicking different menu items
        //
        if (arg0.getSource() == jMenuItemHelpAbout) {
            About a = new About();
            a.setEdited(false);
        }
        
        if (arg0.getSource() == jMenuItemFileCloseAll) {
            this.closeAll();
        }

        //        if (arg0.getSource() == jMenuItemEditAddLibraries) {
        //            showLibraries(FrameworkWizardUI.ADD, IDefaultMutableTreeNode.LIBRARIES, -1);
        //        }
        //
        //        if (arg0.getSource() == jMenuItemEditAddLibraryClassDefinitions) {
        //            showLibraryClassDefinitions(FrameworkWizardUI.ADD, IDefaultMutableTreeNode.LIBRARYCLASSDEFINITIONS);
        //        }
        //
        //        if (arg0.getSource() == jMenuItemEditAddSourceFiles) {
        //            showSourceFiles(FrameworkWizardUI.ADD, IDefaultMutableTreeNode.SOURCEFILES, -1);
        //        }
        //
        //        if (arg0.getSource() == jMenuItemEditAddIncludes) {
        //            showIncludes(FrameworkWizardUI.ADD, IDefaultMutableTreeNode.INCLUDES, -1);
        //        }
        //
        //        if (arg0.getSource() == jMenuItemEditAddProtocols) {
        //            showProtocols(FrameworkWizardUI.ADD, IDefaultMutableTreeNode.PROTOCOLS, -1);
        //        }
        //
        //        if (arg0.getSource() == jMenuItemEditAddEvents) {
        //            showEvents(FrameworkWizardUI.ADD, IDefaultMutableTreeNode.EVENTS, -1);
        //        }
        //
        //        if (arg0.getSource() == jMenuItemEditAddHobs) {
        //            showHobs(FrameworkWizardUI.ADD, IDefaultMutableTreeNode.HOBS, -1);
        //        }
        //
        //        if (arg0.getSource() == jMenuItemEditAddPPIs) {
        //            showPpis(FrameworkWizardUI.ADD, IDefaultMutableTreeNode.PPIS, -1);
        //        }
        //
        //        if (arg0.getSource() == jMenuItemEditAddVariables) {
        //            showVariables(FrameworkWizardUI.ADD, IDefaultMutableTreeNode.VARIABLES, -1);
        //        }
        //
        //        if (arg0.getSource() == jMenuItemEditAddBootModes) {
        //            showBootModes(FrameworkWizardUI.ADD, IDefaultMutableTreeNode.BOOTMODES, -1);
        //        }
        //
        //        if (arg0.getSource() == jMenuItemEditAddSystemTables) {
        //            showSystemTables(FrameworkWizardUI.ADD, IDefaultMutableTreeNode.SYSTEMTABLES, -1);
        //        }
        //
        //        if (arg0.getSource() == jMenuItemEditAddDataHubs) {
        //            showDataHubs(FrameworkWizardUI.ADD, IDefaultMutableTreeNode.DATAHUBS, -1);
        //        }
        //
        //        if (arg0.getSource() == jMenuItemEditAddFormsets) {
        //            showFormsets(FrameworkWizardUI.ADD, IDefaultMutableTreeNode.FORMSETS, -1);
        //        }
        //
        //        if (arg0.getSource() == jMenuItemEditAddGuids) {
        //            showGuids(FrameworkWizardUI.ADD, IDefaultMutableTreeNode.GUIDS, -1);
        //        }
        //
        //        if (arg0.getSource() == jMenuItemEditAddExterns) {
        //            showExterns(FrameworkWizardUI.ADD, IDefaultMutableTreeNode.EXTERNS, -1);
        //        }
        //
        //        if (arg0.getSource() == jMenuItemEditAddPCDs) {
        //            showPCDs(FrameworkWizardUI.ADD, IDefaultMutableTreeNode.PCDS, -1);
        //        }

        if (arg0.getSource() == jMenuItemModuleNewModule) {
            this.closeCurrentModule();
            showMsaHeader(FrameworkWizardUI.ADD);
        }

        //
        // Open Msa, Mbd, Lmsa and Lmbd
        //
        if (arg0.getSource() == jMenuItemFileOpen) {
            openFile(1, 1);
        }

        if (arg0.getSource() == jMenuItemModuleOpenModuleBuildDescription) {
            openFile(1, 2);
        }

        if (arg0.getSource() == jMenuItemModuleOpenLibraryModule) {
            openFile(1, 3);
        }

        if (arg0.getSource() == jMenuItemModuleOpenLibraryModuleBuildDescription) {
            openFile(1, 4);
        }

        //
        // Listen popup menu items
        //
        if (arg0.getSource() == jMenuItemPopupAdd) {
            int intCategory = iTree.getSelectCategory();
            int intLocation = iTree.getSelectLoaction();
            addCurrentModule(intCategory, intLocation);
        }

        //        if (arg0.getSource() == jMenuItemPopupUpdate || arg0.getSource() == jMenuItemEditUpdate) {
        //            int intCategory = iTree.getSelectCategory();
        //            int intLocation = iTree.getSelectLoaction();
        //            updateCurrentModule(intCategory, intLocation);
        //        }

        if (arg0.getSource() == jMenuItemPopupDelete || arg0.getSource() == jMenuItemEditDelete) {
            int intCategory = iTree.getSelectCategory();
            int intLocation = iTree.getSelectLoaction();
            deleteCurrentModule(intCategory, intLocation);
        }

        if (arg0.getSource() == jMenuItemFileExit) {
            this.onExit();
        }

        if (arg0.getSource() == jMenuItemFileClose) {
            closeCurrentModule();
        }

        if (arg0.getSource() == jMenuItemFileSaveAs) {
            saveAsCurrentModule();
        }

        if (arg0.getSource() == jMenuItemFileSave) {
            saveCurrentModule();
        }

        if (arg0.getSource() == jButtonOk) {
            save();
        }

        if (arg0.getSource() == jButtonCancel) {

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
            strCurrentPath = this.currentModule
                                               .substring(
                                                          0,
                                                          this.currentModule
                                                                            .lastIndexOf(System
                                                                                               .getProperty("file.separator")));
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
    private void openMsaFile(String strMsaFilePath) throws IOException, XmlException, Exception {
        Log.log("Open Msa", strMsaFilePath);
        File msaFile = new File(strMsaFilePath);
        xmlMsaDoc = (ModuleSurfaceAreaDocument) XmlObject.Factory.parse(msaFile);
        this.currentModule = strMsaFilePath;
        this.saveFileName = strMsaFilePath;
        this.currentModuleType = 1;

        xmlmh = xmlMsaDoc.getModuleSurfaceArea().getMsaHeader();
        xmllcd = xmlMsaDoc.getModuleSurfaceArea().getLibraryClassDefinitions();
        xmlsf = xmlMsaDoc.getModuleSurfaceArea().getSourceFiles();
        xmlpl = xmlMsaDoc.getModuleSurfaceArea().getProtocols();
        xmlen = xmlMsaDoc.getModuleSurfaceArea().getEvents();
        xmlhob = xmlMsaDoc.getModuleSurfaceArea().getHobs();
        xmlppi = xmlMsaDoc.getModuleSurfaceArea().getPPIs();
        xmlvb = xmlMsaDoc.getModuleSurfaceArea().getVariables();
        xmlbm = xmlMsaDoc.getModuleSurfaceArea().getBootModes();
        xmlst = xmlMsaDoc.getModuleSurfaceArea().getSystemTables();
        xmldh = xmlMsaDoc.getModuleSurfaceArea().getDataHubs();
        xmlfs = xmlMsaDoc.getModuleSurfaceArea().getFormsets();
        xmlgu = xmlMsaDoc.getModuleSurfaceArea().getGuids();
        xmlet = xmlMsaDoc.getModuleSurfaceArea().getExterns();
        xmlpcd = xmlMsaDoc.getModuleSurfaceArea().getPCDs();

        this.showMsaHeader(FrameworkWizardUI.VIEW);
        resizeDesktopPanel();
        //reloadTreeAndTable(FrameworkWizardUI.OPENED);
        //jMenuEditAdd.setEnabled(true);
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
                                                                      IDefaultMutableTreeNode.MODULE, false,
                                                                      this.vModuleList.elementAt(index)));
            }
        }

        // Make Package Description
        dmtnPackageDescription = new IDefaultMutableTreeNode("PackageDescription", -1, -1);
        if (this.vPackageList.size() > 0) {
            for (int index = 0; index < this.vPackageList.size(); index++) {
                dmtnPackageDescription.add(new IDefaultMutableTreeNode(this.vPackageList.elementAt(index).getName(),
                                                                       IDefaultMutableTreeNode.PACKAGE, false,
                                                                       this.vPackageList.elementAt(index)));
            }
        }

        // Make Platform Description
        dmtnPlatformDescription = new IDefaultMutableTreeNode("PlatformDescription", -1, -1);
        if (this.vPlatformList.size() > 0) {
            for (int index = 0; index < this.vPlatformList.size(); index++) {
                dmtnPlatformDescription.add(new IDefaultMutableTreeNode(this.vPlatformList.elementAt(index).getName(),
                                                                        IDefaultMutableTreeNode.PLATFORM, false,
                                                                        this.vPlatformList.elementAt(index)));
            }
        }

        dmtnRoot.add(dmtnModuleDescription);
        dmtnRoot.add(dmtnPackageDescription);
        dmtnRoot.add(dmtnPlatformDescription);
        iTree = new ITree(dmtnRoot);
        iTree.addMouseListener(this);
        jScrollPaneTree.setViewportView(iTree);
    }

    /**
     Create the tree to display all components of current open file.
     First to check if the component is null or not
     If not null, hangs it to the tree
     If null, skip it
     
     **/
    private void makeTree() {
        iTree.removeAll();

        //
        //Make an empty tree when closing
        //
        if (this.currentModuleType == 0) {
            makeEmptyTree();
            return;
        }

        //
        //Msa File
        //
        if (this.currentModuleType == 1) {
            //
            //Add MsaHeader Node
            //
            if (xmlmh != null) {
                dmtnRoot = new IDefaultMutableTreeNode(xmlmh.getBaseName(), IDefaultMutableTreeNode.MSA_HEADER,
                                                       IDefaultMutableTreeNode.OPERATION_UPDATE_DELETE);
            } else {
                makeEmptyTree();
                return;
            }

            //
            //Add LibraryClassDefinitions Node
            //
            if (xmllcd != null && xmllcd.getLibraryClassList().size() > 0) {
                IDefaultMutableTreeNode libraryClassDefinitions = new IDefaultMutableTreeNode(
                                                                                              "Library Class Definitions",
                                                                                              IDefaultMutableTreeNode.LIBRARYCLASSDEFINITIONS,
                                                                                              IDefaultMutableTreeNode.OPERATION_UPDATE_DELETE);
                for (int indexI = 0; indexI < xmllcd.getLibraryClassList().size(); indexI++) {
                    libraryClassDefinitions
                                           .add(new IDefaultMutableTreeNode(
                                                                            xmllcd.getLibraryClassArray(indexI)
                                                                                  .getStringValue(),
                                                                            IDefaultMutableTreeNode.LIBRARY_CLASS_DEFINITION,
                                                                            IDefaultMutableTreeNode.OPERATION_NULL));
                }
                dmtnRoot.add(libraryClassDefinitions);
            }
        }

        //
        //Add SourceFiles Node
        //
        //        if (xmlsf != null) {
        //            IDefaultMutableTreeNode sourceFiles = new IDefaultMutableTreeNode(
        //                                                                              "Source Files",
        //                                                                              IDefaultMutableTreeNode.SOURCEFILES,
        //                                                                              IDefaultMutableTreeNode.OPERATION_ADD_DELETE);
        //            if (xmlsf.getArchList().size() > 0) {
        //                IDefaultMutableTreeNode sourceFilesArch = new IDefaultMutableTreeNode(
        //                                                                                      "Arch",
        //                                                                                      IDefaultMutableTreeNode.SOURCEFILES_ARCH,
        //                                                                                      IDefaultMutableTreeNode.OPERATION_ADD_DELETE);
        //                for (int indexI = 0; indexI < xmlsf.getArchList().size(); indexI++) {
        //                    sourceFilesArch
        //                                   .add(new IDefaultMutableTreeNode(
        //                                                                    xmlsf.getArchArray(indexI).getArchType().toString(),
        //                                                                    IDefaultMutableTreeNode.SOURCEFILES_ARCH_ITEM,
        //                                                                    IDefaultMutableTreeNode.OPERATION_UPDATE_DELETE,
        //                                                                    indexI));
        //                }
        //                sourceFiles.add(sourceFilesArch);
        //            }
        //            if (xmlsf.getFilenameList().size() > 0) {
        //                IDefaultMutableTreeNode sourceFilesFileName = new IDefaultMutableTreeNode(
        //                                                                                          "File Name",
        //                                                                                          IDefaultMutableTreeNode.SOURCEFILES_FILENAME,
        //                                                                                          IDefaultMutableTreeNode.OPERATION_ADD_UPDATE_DELETE);
        //                for (int indexI = 0; indexI < xmlsf.getFilenameList().size(); indexI++) {
        //                    sourceFilesFileName
        //                                       .add(new IDefaultMutableTreeNode(
        //                                                                        xmlsf.getFilenameArray(indexI).getStringValue(),
        //                                                                        IDefaultMutableTreeNode.SOURCEFILES_FILENAME_ITEM,
        //                                                                        IDefaultMutableTreeNode.OPERATION_DELETE));
        //                }
        //                sourceFiles.add(sourceFilesFileName);
        //            }
        //            dmtnRoot.add(sourceFiles);
        //        }
        //
        //        //
        //        //Add includes
        //        //
        //        if (xmlic != null) {
        //            IDefaultMutableTreeNode includes = new IDefaultMutableTreeNode("Includes",
        //                                                                           IDefaultMutableTreeNode.INCLUDES,
        //                                                                           IDefaultMutableTreeNode.OPERATION_ADD_DELETE);
        //            if (xmlic.getArchList().size() > 0) {
        //                IDefaultMutableTreeNode includesArch = new IDefaultMutableTreeNode(
        //                                                                                   "Arch",
        //                                                                                   IDefaultMutableTreeNode.INCLUDES_ARCH,
        //                                                                                   IDefaultMutableTreeNode.OPERATION_ADD_DELETE);
        //                for (int indexI = 0; indexI < xmlic.getArchList().size(); indexI++) {
        //                    includesArch.add(new IDefaultMutableTreeNode(xmlic.getArchArray(indexI).getArchType().toString(),
        //                                                                 IDefaultMutableTreeNode.INCLUDES_ARCH_ITEM,
        //                                                                 IDefaultMutableTreeNode.OPERATION_UPDATE_DELETE,
        //                                                                 indexI));
        //                }
        //                includes.add(includesArch);
        //            }
        //            if (xmlic.getPackageNameList().size() > 0) {
        //                IDefaultMutableTreeNode includesPackageName = new IDefaultMutableTreeNode(
        //                                                                                          "Package Name",
        //                                                                                          IDefaultMutableTreeNode.INCLUDES_PACKAGENAME,
        //                                                                                          IDefaultMutableTreeNode.OPERATION_ADD_UPDATE_DELETE);
        //                for (int indexI = 0; indexI < xmlic.getPackageNameList().size(); indexI++) {
        //                    includesPackageName
        //                                       .add(new IDefaultMutableTreeNode(
        //                                                                        xmlic.getPackageNameArray(indexI)
        //                                                                             .getStringValue(),
        //                                                                        IDefaultMutableTreeNode.INCLUDES_PACKAGENAME_ITEM,
        //                                                                        IDefaultMutableTreeNode.OPERATION_DELETE));
        //                }
        //                includes.add(includesPackageName);
        //            }
        //            dmtnRoot.add(includes);
        //        }

        //
        //Add protocols
        //
        if (xmlpl != null) {
            IDefaultMutableTreeNode dmtnProtocols = new IDefaultMutableTreeNode(
                                                                                "Protocols",
                                                                                IDefaultMutableTreeNode.PROTOCOLS,
                                                                                IDefaultMutableTreeNode.OPERATION_ADD_DELETE);
            IDefaultMutableTreeNode dmtnProtocol = new IDefaultMutableTreeNode(
                                                                               "Protocol",
                                                                               IDefaultMutableTreeNode.PROTOCOLS_PROTOCOL,
                                                                               IDefaultMutableTreeNode.OPERATION_ADD_DELETE);
            IDefaultMutableTreeNode dmtnProtocolNotify = new IDefaultMutableTreeNode(
                                                                                     "Protocol Notify",
                                                                                     IDefaultMutableTreeNode.PROTOCOLS_PROTOCOLNOTIFY,
                                                                                     IDefaultMutableTreeNode.OPERATION_ADD_DELETE);
            if (xmlpl.getProtocolList().size() > 0) {
                for (int indexI = 0; indexI < xmlpl.getProtocolList().size(); indexI++) {
                    dmtnProtocol.add(new IDefaultMutableTreeNode(xmlpl.getProtocolArray(indexI).getStringValue(),
                                                                 IDefaultMutableTreeNode.PROTOCOLS_PROTOCOL_ITEM,
                                                                 IDefaultMutableTreeNode.OPERATION_UPDATE_DELETE,
                                                                 indexI));
                }
                dmtnProtocols.add(dmtnProtocol);
            }
            if (xmlpl.getProtocolNotifyList().size() > 0) {
                for (int indexI = 0; indexI < xmlpl.getProtocolNotifyList().size(); indexI++) {
                    dmtnProtocolNotify
                                      .add(new IDefaultMutableTreeNode(
                                                                       xmlpl.getProtocolNotifyArray(indexI)
                                                                            .getStringValue(),
                                                                       IDefaultMutableTreeNode.PROTOCOLS_PROTOCOLNOTIFY_ITEM,
                                                                       IDefaultMutableTreeNode.OPERATION_UPDATE_DELETE,
                                                                       indexI));
                }
                dmtnProtocols.add(dmtnProtocolNotify);
            }
            dmtnRoot.add(dmtnProtocols);
        }

        //
        //Add events
        //
        if (xmlen != null) {
            IDefaultMutableTreeNode dmtnEvents = new IDefaultMutableTreeNode(
                                                                             "Events",
                                                                             IDefaultMutableTreeNode.EVENTS,
                                                                             IDefaultMutableTreeNode.OPERATION_ADD_DELETE);
            IDefaultMutableTreeNode dmtnCreateEvents = new IDefaultMutableTreeNode(
                                                                                   "Create",
                                                                                   IDefaultMutableTreeNode.EVENTS_CREATEEVENTS,
                                                                                   IDefaultMutableTreeNode.OPERATION_ADD_DELETE);
            IDefaultMutableTreeNode dmtnSignalEvents = new IDefaultMutableTreeNode(
                                                                                   "Signal",
                                                                                   IDefaultMutableTreeNode.EVENTS_SIGNALEVENTS,
                                                                                   IDefaultMutableTreeNode.OPERATION_ADD_DELETE);
            if (xmlen.getCreateEvents() != null && xmlen.getCreateEvents().getEventList().size() > 0) {
                for (int indexI = 0; indexI < xmlen.getCreateEvents().getEventList().size(); indexI++) {
                    dmtnCreateEvents.add(new IDefaultMutableTreeNode(xmlen.getCreateEvents().getEventArray(indexI)
                                                                          .getCName(),
                                                                     IDefaultMutableTreeNode.EVENTS_CREATEEVENTS_ITEM,
                                                                     IDefaultMutableTreeNode.OPERATION_UPDATE_DELETE,
                                                                     indexI));
                }
                dmtnEvents.add(dmtnCreateEvents);
            }
            if (xmlen.getSignalEvents() != null && xmlen.getSignalEvents().getEventList().size() > 0) {
                for (int indexI = 0; indexI < xmlen.getSignalEvents().getEventList().size(); indexI++) {
                    dmtnSignalEvents.add(new IDefaultMutableTreeNode(xmlen.getSignalEvents().getEventArray(indexI)
                                                                          .getCName(),
                                                                     IDefaultMutableTreeNode.EVENTS_SIGNALEVENTS_ITEM,
                                                                     IDefaultMutableTreeNode.OPERATION_UPDATE_DELETE,
                                                                     indexI));
                }
                dmtnEvents.add(dmtnSignalEvents);
            }
            dmtnRoot.add(dmtnEvents);
        }

        //
        //Add hobs
        //
        if (xmlhob != null) {
            IDefaultMutableTreeNode dmtnHobs = new IDefaultMutableTreeNode("Hobs", IDefaultMutableTreeNode.HOBS,
                                                                           IDefaultMutableTreeNode.OPERATION_ADD_DELETE);
            if (xmlhob.getHobList().size() > 0) {
                for (int indexI = 0; indexI < xmlhob.getHobList().size(); indexI++) {
                    dmtnHobs.add(new IDefaultMutableTreeNode(xmlhob.getHobArray(indexI).getName(),
                                                             IDefaultMutableTreeNode.HOBS_HOB_ITEM,
                                                             IDefaultMutableTreeNode.OPERATION_UPDATE_DELETE, indexI));
                }
            }
            dmtnRoot.add(dmtnHobs);
        }

        //
        //Add ppis
        //
        if (xmlppi != null) {
            IDefaultMutableTreeNode dmtnPpis = new IDefaultMutableTreeNode("Ppis", IDefaultMutableTreeNode.PPIS,
                                                                           IDefaultMutableTreeNode.OPERATION_ADD_DELETE);
            IDefaultMutableTreeNode dmtnPpi = new IDefaultMutableTreeNode("Ppi", IDefaultMutableTreeNode.PPIS_PPI,
                                                                          IDefaultMutableTreeNode.OPERATION_ADD_DELETE);
            IDefaultMutableTreeNode dmtnPpiNotify = new IDefaultMutableTreeNode(
                                                                                "Ppi Notify",
                                                                                IDefaultMutableTreeNode.PPIS_PPINOTIFY,
                                                                                IDefaultMutableTreeNode.OPERATION_ADD_DELETE);
            if (xmlppi.getPpiList().size() > 0) {
                for (int indexI = 0; indexI < xmlppi.getPpiList().size(); indexI++) {
                    dmtnPpi.add(new IDefaultMutableTreeNode(xmlppi.getPpiArray(indexI).getStringValue(),
                                                            IDefaultMutableTreeNode.PPIS_PPI_ITEM,
                                                            IDefaultMutableTreeNode.OPERATION_UPDATE_DELETE, indexI));
                }
                dmtnPpis.add(dmtnPpi);
            }
            if (xmlppi.getPpiNotifyList().size() > 0) {
                for (int indexI = 0; indexI < xmlppi.getPpiNotifyList().size(); indexI++) {
                    dmtnPpiNotify.add(new IDefaultMutableTreeNode(xmlppi.getPpiNotifyArray(indexI).getStringValue(),
                                                                  IDefaultMutableTreeNode.PPIS_PPINOTIFY_ITEM,
                                                                  IDefaultMutableTreeNode.OPERATION_UPDATE_DELETE,
                                                                  indexI));
                }
                dmtnPpis.add(dmtnPpiNotify);
            }
            dmtnRoot.add(dmtnPpis);
        }

        //
        //Add variables
        //
        if (xmlvb != null) {
            IDefaultMutableTreeNode dmtnVariables = new IDefaultMutableTreeNode(
                                                                                "Variables",
                                                                                IDefaultMutableTreeNode.VARIABLES,
                                                                                IDefaultMutableTreeNode.OPERATION_ADD_DELETE);
            if (xmlvb.getVariableList().size() > 0) {
                for (int indexI = 0; indexI < xmlvb.getVariableList().size(); indexI++) {
                    dmtnVariables.add(new IDefaultMutableTreeNode(xmlvb.getVariableArray(indexI).getString(),
                                                                  IDefaultMutableTreeNode.VARIABLES_VARIABLE_ITEM,
                                                                  IDefaultMutableTreeNode.OPERATION_UPDATE_DELETE,
                                                                  indexI));
                }
            }
            dmtnRoot.add(dmtnVariables);
        }

        //
        //Add bootmodes
        //
        if (xmlbm != null) {
            IDefaultMutableTreeNode dmtnBootModes = new IDefaultMutableTreeNode(
                                                                                "BootModes",
                                                                                IDefaultMutableTreeNode.BOOTMODES,
                                                                                IDefaultMutableTreeNode.OPERATION_ADD_DELETE);
            if (xmlbm.getBootModeList().size() > 0) {
                for (int indexI = 0; indexI < xmlbm.getBootModeList().size(); indexI++) {
                    dmtnBootModes.add(new IDefaultMutableTreeNode(xmlbm.getBootModeArray(indexI).getBootModeName()
                                                                       .toString(),
                                                                  IDefaultMutableTreeNode.BOOTMODES_BOOTMODE_ITEM,
                                                                  IDefaultMutableTreeNode.OPERATION_UPDATE_DELETE,
                                                                  indexI));
                }
            }
            dmtnRoot.add(dmtnBootModes);
        }

        //
        //Add systemtables
        //
        if (xmlst != null) {
            IDefaultMutableTreeNode dmtnSystemTables = new IDefaultMutableTreeNode(
                                                                                   "SystemTables",
                                                                                   IDefaultMutableTreeNode.SYSTEMTABLES,
                                                                                   IDefaultMutableTreeNode.OPERATION_ADD_DELETE);
            if (xmlst.getSystemTableList().size() > 0) {
                for (int indexI = 0; indexI < xmlst.getSystemTableList().size(); indexI++) {
                    dmtnSystemTables
                                    .add(new IDefaultMutableTreeNode(
                                                                     xmlst.getSystemTableArray(indexI).getEntry(),
                                                                     IDefaultMutableTreeNode.SYSTEMTABLES_SYSTEMTABLE_ITEM,
                                                                     IDefaultMutableTreeNode.OPERATION_UPDATE_DELETE,
                                                                     indexI));
                }
            }
            dmtnRoot.add(dmtnSystemTables);
        }

        //
        //Add datahubs
        //
        if (xmldh != null) {
            IDefaultMutableTreeNode dmtnDataHubs = new IDefaultMutableTreeNode(
                                                                               "DataHubs",
                                                                               IDefaultMutableTreeNode.DATAHUBS,
                                                                               IDefaultMutableTreeNode.OPERATION_ADD_DELETE);
            if (xmldh.getDataHubRecordList().size() > 0) {
                for (int indexI = 0; indexI < xmldh.getDataHubRecordList().size(); indexI++) {
                    dmtnDataHubs.add(new IDefaultMutableTreeNode(xmldh.getDataHubRecordArray(indexI).getStringValue(),
                                                                 IDefaultMutableTreeNode.DATAHUBS_DATAHUB_ITEM,
                                                                 IDefaultMutableTreeNode.OPERATION_UPDATE_DELETE,
                                                                 indexI));
                }
            }
            dmtnRoot.add(dmtnDataHubs);
        }

        //
        //Add formsets
        //
        if (xmlfs != null) {
            IDefaultMutableTreeNode dmtnFormsets = new IDefaultMutableTreeNode(
                                                                               "Formsets",
                                                                               IDefaultMutableTreeNode.FORMSETS,
                                                                               IDefaultMutableTreeNode.OPERATION_ADD_DELETE);
            if (xmlfs.getFormsetList().size() > 0) {
                for (int indexI = 0; indexI < xmlfs.getFormsetList().size(); indexI++) {
                    dmtnFormsets.add(new IDefaultMutableTreeNode(xmlfs.getFormsetArray(indexI).getStringValue(),
                                                                 IDefaultMutableTreeNode.FORMSETS_FORMSET_ITEM,
                                                                 IDefaultMutableTreeNode.OPERATION_UPDATE_DELETE,
                                                                 indexI));
                }
            }
            dmtnRoot.add(dmtnFormsets);
        }

        //
        //Add guids
        //
        if (xmlgu != null) {
            IDefaultMutableTreeNode dmtnGuids = new IDefaultMutableTreeNode(
                                                                            "Guids",
                                                                            IDefaultMutableTreeNode.GUIDS,
                                                                            IDefaultMutableTreeNode.OPERATION_ADD_DELETE);
            if (xmlgu.getGuidEntryList().size() > 0) {
                for (int indexI = 0; indexI < xmlgu.getGuidEntryList().size(); indexI++) {
                    dmtnGuids.add(new IDefaultMutableTreeNode(xmlgu.getGuidEntryArray(indexI).getCName(),
                                                              IDefaultMutableTreeNode.GUIDS_GUIDENTRY_ITEM,
                                                              IDefaultMutableTreeNode.OPERATION_UPDATE_DELETE, indexI));
                }
            }
            dmtnRoot.add(dmtnGuids);
        }

        //
        //Add externs
        //
        if (xmlet != null) {
            IDefaultMutableTreeNode dmtnExterns = new IDefaultMutableTreeNode(
                                                                              "Externs",
                                                                              IDefaultMutableTreeNode.EXTERNS,
                                                                              IDefaultMutableTreeNode.OPERATION_ADD_DELETE);
            if (xmlet.getExternList().size() > 0) {
                for (int indexI = 0; indexI < xmlet.getExternList().size(); indexI++) {
                    dmtnExterns
                               .add(new IDefaultMutableTreeNode("Extern " + Integer.valueOf(indexI + 1),
                                                                IDefaultMutableTreeNode.EXTERNS_EXTERN_ITEM,
                                                                IDefaultMutableTreeNode.OPERATION_UPDATE_DELETE, indexI));
                }
            }
            dmtnRoot.add(dmtnExterns);
        }

        //
        //Add pcds
        //
        if (xmlpcd != null) {
            IDefaultMutableTreeNode dmtnPCDs = new IDefaultMutableTreeNode("PCDs", IDefaultMutableTreeNode.PCDS,
                                                                           IDefaultMutableTreeNode.OPERATION_ADD_DELETE);
            if (xmlpcd.getPcdDataList().size() > 0) {
                for (int indexI = 0; indexI < xmlpcd.getPcdDataList().size(); indexI++) {
                    dmtnPCDs.add(new IDefaultMutableTreeNode(xmlpcd.getPcdDataArray(indexI).getCName(),
                                                             IDefaultMutableTreeNode.PCDS_PCDDATA_ITEM,
                                                             IDefaultMutableTreeNode.OPERATION_UPDATE_DELETE, indexI));
                }
            }
            dmtnRoot.add(dmtnPCDs);
        }

        iTree = new ITree(dmtnRoot);
        iTree.addMouseListener(this);
        iTree.addTreeSelectionListener(this);
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
        viewCurrentModule();
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
        this.currentNodeType = 0;

        xmlMsaDoc = null;
        xmlmh = null;
        xmllcd = null;
        xmlsf = null;
        xmlpl = null;
        xmlen = null;
        xmlhob = null;
        xmlppi = null;
        xmlvb = null;
        xmlbm = null;
        xmlst = null;
        xmldh = null;
        xmlfs = null;
        xmlgu = null;
        xmlet = null;
        xmlpcd = null;
    }

    /**
     Execute add operation for current node 
     
     @param intCategory The category of current node
     @param intLocation The location of current node
     
     **/
    private void addCurrentModule(int intCategory, int intLocation) {
        //
        //Add new sourcefiles
        //
        if (intCategory == IDefaultMutableTreeNode.SOURCEFILES
            || intCategory == IDefaultMutableTreeNode.SOURCEFILES_FILENAME
            || intCategory == IDefaultMutableTreeNode.SOURCEFILES_ARCH) {
            showSourceFiles(FrameworkWizardUI.ADD, intCategory, -1);
        }

        //
        //Add new protocols
        //
        if (intCategory == IDefaultMutableTreeNode.PROTOCOLS
            || intCategory == IDefaultMutableTreeNode.PROTOCOLS_PROTOCOL
            || intCategory == IDefaultMutableTreeNode.PROTOCOLS_PROTOCOLNOTIFY) {
            showProtocols(FrameworkWizardUI.ADD, intCategory, -1);
        }

        //
        //Add new events
        //
        if (intCategory == IDefaultMutableTreeNode.EVENTS || intCategory == IDefaultMutableTreeNode.EVENTS_CREATEEVENTS
            || intCategory == IDefaultMutableTreeNode.EVENTS_SIGNALEVENTS) {
            showEvents(FrameworkWizardUI.ADD, intCategory, -1);
        }

        //
        //Add new hobs
        //
        if (intCategory == IDefaultMutableTreeNode.HOBS || intCategory == IDefaultMutableTreeNode.HOBS_HOB_ITEM) {
            showHobs(FrameworkWizardUI.ADD, intCategory, -1);
        }

        //
        //Add new ppis
        //
        if (intCategory == IDefaultMutableTreeNode.PPIS || intCategory == IDefaultMutableTreeNode.PPIS_PPI
            || intCategory == IDefaultMutableTreeNode.PPIS_PPINOTIFY) {
            showPpis(FrameworkWizardUI.ADD, intCategory, -1);
        }

        //
        //Add new variables
        //
        if (intCategory == IDefaultMutableTreeNode.VARIABLES
            || intCategory == IDefaultMutableTreeNode.VARIABLES_VARIABLE_ITEM) {
            showVariables(FrameworkWizardUI.ADD, intCategory, -1);
        }

        //
        //Add new BootModes
        //
        if (intCategory == IDefaultMutableTreeNode.BOOTMODES
            || intCategory == IDefaultMutableTreeNode.BOOTMODES_BOOTMODE_ITEM) {
            showBootModes(FrameworkWizardUI.ADD, intCategory, -1);
        }

        //
        //Add new SystemTables
        //
        if (intCategory == IDefaultMutableTreeNode.SYSTEMTABLES
            || intCategory == IDefaultMutableTreeNode.SYSTEMTABLES_SYSTEMTABLE_ITEM) {
            showSystemTables(FrameworkWizardUI.ADD, intCategory, -1);
        }

        //
        //Add new DataHubs
        //
        if (intCategory == IDefaultMutableTreeNode.DATAHUBS
            || intCategory == IDefaultMutableTreeNode.DATAHUBS_DATAHUB_ITEM) {
            showDataHubs(FrameworkWizardUI.ADD, intCategory, -1);
        }

        //
        //Add new Formsets
        //
        if (intCategory == IDefaultMutableTreeNode.FORMSETS
            || intCategory == IDefaultMutableTreeNode.FORMSETS_FORMSET_ITEM) {
            showFormsets(FrameworkWizardUI.ADD, intCategory, -1);
        }

        //
        //Add new Guids
        //
        if (intCategory == IDefaultMutableTreeNode.GUIDS || intCategory == IDefaultMutableTreeNode.GUIDS_GUIDENTRY_ITEM) {
            showGuids(FrameworkWizardUI.ADD, intCategory, -1);
        }

        //
        //Add new Externs
        //
        if (intCategory == IDefaultMutableTreeNode.EXTERNS
            || intCategory == IDefaultMutableTreeNode.EXTERNS_EXTERN_ITEM) {
            showExterns(FrameworkWizardUI.ADD, intCategory, -1);
        }

        //
        //Add new PCDs
        //
        if (intCategory == IDefaultMutableTreeNode.PCDS || intCategory == IDefaultMutableTreeNode.PCDS_PCDDATA_ITEM) {
            showPCDs(FrameworkWizardUI.ADD, intCategory, -1);
        }
    }

    /**
     Execute delete operation of current node
     
     @param intCategory The category of current node
     @param intLocation The location of current node
     
     **/
    private void deleteCurrentModule(int intCategory, int intLocation) {
        //
        // Delete Msa Header
        //
        if (intCategory == IDefaultMutableTreeNode.MSA_HEADER || intCategory == IDefaultMutableTreeNode.MBD_HEADER
            || intCategory == IDefaultMutableTreeNode.MLSA_HEADER || intCategory == IDefaultMutableTreeNode.MLBD_HEADER) {
            if (JOptionPane.showConfirmDialog(null, "The module will be deleted permanently, do you want to continue?") == JOptionPane.YES_OPTION) {
                try {
                    File f = new File(currentModule);
                    f.delete();
                    closeCurrentModule();
                } catch (Exception e) {
                    Log.err("Delete " + currentModule, e.getMessage());
                }
            } else {
                return;
            }
        }

        //
        //Delete LIBRARY CLASS DEFINITIONS
        //
        if (intCategory == IDefaultMutableTreeNode.LIBRARYCLASSDEFINITIONS) {
            xmllcd = null;
        }

        //
        //Delete Libraries
        //
        //        if (intCategory == IDefaultMutableTreeNode.LIBRARIES) {
        //            xmllib = null;
        //        }
        //        if (intCategory == IDefaultMutableTreeNode.LIBRARIES_LIBRARY) {
        //            for (int indexI = xmllib.getLibraryList().size() - 1; indexI > -1; indexI--) {
        //                xmllib.removeLibrary(indexI);
        //            }
        //            if (xmllib.getArchList().size() < 1 && xmllib.getLibraryList().size() < 1) {
        //                xmllib = null;
        //            }
        //        }
        //        if (intCategory == IDefaultMutableTreeNode.LIBRARIES_ARCH) {
        //            for (int indexI = xmllib.getArchList().size() - 1; indexI > -1; indexI--) {
        //                xmllib.removeArch(indexI);
        //            }
        //            if (xmllib.getArchList().size() < 1 && xmllib.getLibraryList().size() < 1) {
        //                xmllib = null;
        //            }
        //        }
        //        if (intCategory == IDefaultMutableTreeNode.LIBRARIES_ARCH_ITEM) {
        //            xmllib.removeArch(intLocation);
        //        }
        //        if (intCategory == IDefaultMutableTreeNode.LIBRARIES_LIBRARY_ITEM) {
        //            xmllib.removeLibrary(intLocation);
        //        }

        //
        //Delete SourceFiles
        //
        //        if (intCategory == IDefaultMutableTreeNode.SOURCEFILES) {
        //            xmlsf = null;
        //        }
        //        if (intCategory == IDefaultMutableTreeNode.SOURCEFILES_FILENAME) {
        //            for (int indexI = xmlsf.getFilenameList().size() - 1; indexI > -1; indexI--) {
        //                xmlsf.removeFilename(indexI);
        //            }
        //            if (xmlsf.getArchList().size() < 1 && xmlsf.getFilenameList().size() < 1) {
        //                xmlsf = null;
        //            }
        //        }
        //        if (intCategory == IDefaultMutableTreeNode.SOURCEFILES_ARCH) {
        //            for (int indexI = xmlsf.getArchList().size() - 1; indexI > -1; indexI--) {
        //                xmlsf.removeArch(indexI);
        //            }
        //            if (xmlsf.getArchList().size() < 1 && xmlsf.getFilenameList().size() < 1) {
        //                xmlsf = null;
        //            }
        //        }
        //        if (intCategory == IDefaultMutableTreeNode.SOURCEFILES_ARCH_ITEM) {
        //            xmlsf.removeArch(intLocation);
        //        }
        //        if (intCategory == IDefaultMutableTreeNode.SOURCEFILES_FILENAME_ITEM) {
        //            xmlsf.removeFilename(intLocation);
        //        }

        //
        //Delete Protocols
        //
        if (intCategory == IDefaultMutableTreeNode.PROTOCOLS) {
            xmlpl = null;
        }
        if (intCategory == IDefaultMutableTreeNode.PROTOCOLS_PROTOCOL) {
            for (int indexI = xmlpl.getProtocolList().size() - 1; indexI > -1; indexI--) {
                xmlpl.removeProtocol(indexI);
            }
            if (xmlpl.getProtocolList().size() < 1 && xmlpl.getProtocolNotifyList().size() < 1) {
                xmlpl = null;
            }
        }
        if (intCategory == IDefaultMutableTreeNode.PROTOCOLS_PROTOCOLNOTIFY) {
            for (int indexI = xmlpl.getProtocolList().size() - 1; indexI > -1; indexI--) {
                xmlpl.removeProtocolNotify(indexI);
            }
            if (xmlpl.getProtocolList().size() < 1 && xmlpl.getProtocolNotifyList().size() < 1) {
                xmlpl = null;
            }
        }
        if (intCategory == IDefaultMutableTreeNode.PROTOCOLS_PROTOCOL_ITEM) {
            xmlpl.removeProtocol(intLocation);
        }
        if (intCategory == IDefaultMutableTreeNode.PROTOCOLS_PROTOCOLNOTIFY_ITEM) {
            xmlpl.removeProtocolNotify(intLocation);
        }

        //
        //Delete Events
        //
        if (intCategory == IDefaultMutableTreeNode.EVENTS) {
            xmlen = null;
        }
        if (intCategory == IDefaultMutableTreeNode.EVENTS_CREATEEVENTS) {
            for (int indexI = xmlen.getCreateEvents().getEventList().size() - 1; indexI > -1; indexI--) {
                xmlen.getCreateEvents().removeEvent(indexI);
            }
            if (xmlen.getCreateEvents().getEventList().size() < 1 && xmlen.getSignalEvents().getEventList().size() < 1) {
                xmlen = null;
            }
        }
        if (intCategory == IDefaultMutableTreeNode.EVENTS_SIGNALEVENTS) {
            for (int indexI = xmlen.getSignalEvents().getEventList().size() - 1; indexI > -1; indexI--) {
                xmlen.getSignalEvents().removeEvent(indexI);
            }
            if (xmlen.getCreateEvents().getEventList().size() < 1 && xmlen.getSignalEvents().getEventList().size() < 1) {
                xmlen = null;
            }
        }
        if (intCategory == IDefaultMutableTreeNode.EVENTS_CREATEEVENTS_ITEM) {
            xmlen.getCreateEvents().removeEvent(intLocation);
        }
        if (intCategory == IDefaultMutableTreeNode.EVENTS_SIGNALEVENTS_ITEM) {
            xmlen.getSignalEvents().removeEvent(intLocation);
        }

        //
        //Delete Hobs
        //
        if (intCategory == IDefaultMutableTreeNode.HOBS) {
            xmlhob = null;
        }
        if (intCategory == IDefaultMutableTreeNode.HOBS_HOB_ITEM) {
            xmlhob.removeHob(intLocation);
            if (xmlhob.getHobList().size() < 1) {
                xmlhob = null;
            }
        }

        //
        //Delete Ppis
        //
        if (intCategory == IDefaultMutableTreeNode.PPIS) {
            xmlppi = null;
        }
        if (intCategory == IDefaultMutableTreeNode.PPIS_PPI) {
            for (int indexI = xmlppi.getPpiList().size() - 1; indexI > -1; indexI--) {
                xmlppi.removePpi(indexI);
            }
            if (xmlppi.getPpiList().size() < 1 && xmlppi.getPpiNotifyList().size() < 1) {
                xmlppi = null;
            }
        }
        if (intCategory == IDefaultMutableTreeNode.PPIS_PPINOTIFY) {
            for (int indexI = xmlppi.getPpiNotifyList().size() - 1; indexI > -1; indexI--) {
                xmlppi.removePpiNotify(indexI);
            }
            if (xmlppi.getPpiList().size() < 1 && xmlppi.getPpiNotifyList().size() < 1) {
                xmlppi = null;
            }
        }
        if (intCategory == IDefaultMutableTreeNode.PPIS_PPI_ITEM) {
            xmlppi.removePpi(intLocation);
        }
        if (intCategory == IDefaultMutableTreeNode.PPIS_PPINOTIFY_ITEM) {
            xmlppi.removePpiNotify(intLocation);
        }

        //
        //Delete Variables
        //
        if (intCategory == IDefaultMutableTreeNode.VARIABLES) {
            xmlvb = null;
        }
        if (intCategory == IDefaultMutableTreeNode.VARIABLES_VARIABLE_ITEM) {
            xmlvb.removeVariable(intLocation);
            if (xmlvb.getVariableList().size() < 1) {
                xmlvb = null;
            }
        }

        //
        //Delete BootModes
        //
        if (intCategory == IDefaultMutableTreeNode.BOOTMODES) {
            xmlbm = null;
        }
        if (intCategory == IDefaultMutableTreeNode.BOOTMODES_BOOTMODE_ITEM) {
            xmlbm.removeBootMode(intLocation);
            if (xmlbm.getBootModeList().size() < 1) {
                xmlbm = null;
            }
        }

        //
        //Delete SystemTables
        //
        if (intCategory == IDefaultMutableTreeNode.SYSTEMTABLES) {
            xmlst = null;
        }
        if (intCategory == IDefaultMutableTreeNode.SYSTEMTABLES_SYSTEMTABLE_ITEM) {
            xmlst.removeSystemTable(intLocation);
            if (xmlst.getSystemTableList().size() < 1) {
                xmlst = null;
            }
        }

        //
        //Delete DataHubs
        //
        if (intCategory == IDefaultMutableTreeNode.DATAHUBS) {
            xmldh = null;
        }
        if (intCategory == IDefaultMutableTreeNode.DATAHUBS_DATAHUB_ITEM) {
            xmldh.removeDataHubRecord(intLocation);
            if (xmldh.getDataHubRecordList().size() < 1) {
                xmldh = null;
            }
        }

        //
        //Delete Formsets
        //
        if (intCategory == IDefaultMutableTreeNode.FORMSETS) {
            xmlfs = null;
        }
        if (intCategory == IDefaultMutableTreeNode.FORMSETS_FORMSET_ITEM) {
            xmlfs.removeFormset(intLocation);
            if (xmlfs.getFormsetList().size() < 1) {
                xmlfs = null;
            }
        }

        //
        //Delete Guids
        //
        if (intCategory == IDefaultMutableTreeNode.GUIDS) {
            xmlgu = null;
        }
        if (intCategory == IDefaultMutableTreeNode.GUIDS_GUIDENTRY_ITEM) {
            xmlgu.removeGuidEntry(intLocation);
            if (xmlgu.getGuidEntryList().size() < 1) {
                xmlgu = null;
            }
        }

        //
        //Delete Externs
        //
        if (intCategory == IDefaultMutableTreeNode.EXTERNS) {
            xmlet = null;
        }
        if (intCategory == IDefaultMutableTreeNode.EXTERNS_EXTERN_ITEM) {
            xmlet.removeExtern(intLocation);
            if (xmlet.getExternList().size() < 1) {
                xmlet = null;
            }
        }

        //
        //Delete PCDs
        //
        if (intCategory == IDefaultMutableTreeNode.PCDS) {
            xmlpcd = null;
        }
        if (intCategory == IDefaultMutableTreeNode.PCDS_PCDDATA_ITEM) {
            xmlpcd.removePcdData(intLocation);
            if (xmlpcd.getPcdDataList().size() < 1) {
                xmlpcd = null;
            }
        }
        this.cleanDesktopPane();
        reloadTreeAndTable(UPDATE_WITH_CHANGE);
    }

    /**
     View current Module
     
     **/
    private void viewCurrentModule() {
        int intCategory = iTree.getSelectCategory();
        int intLocation = iTree.getSelectLoaction();
        //
        //View Msa Header
        //
        if (intCategory == IDefaultMutableTreeNode.MSA_HEADER) {
            showMsaHeader(FrameworkWizardUI.VIEW);
        }

        //
        //View LIBRARY CLASS DEFINITIONS
        //
        if (intCategory == IDefaultMutableTreeNode.LIBRARYCLASSDEFINITIONS) {
            showLibraryClassDefinitions(FrameworkWizardUI.VIEW, intCategory);
        }

        //
        //View Source Files
        //
        if (intCategory == IDefaultMutableTreeNode.SOURCEFILES_FILENAME
            || intCategory == IDefaultMutableTreeNode.SOURCEFILES_ARCH_ITEM) {
            showSourceFiles(FrameworkWizardUI.VIEW, intCategory, intLocation);
        }

        //
        //View Protocols
        //
        if (intCategory == IDefaultMutableTreeNode.PROTOCOLS_PROTOCOL_ITEM
            || intCategory == IDefaultMutableTreeNode.PROTOCOLS_PROTOCOLNOTIFY_ITEM) {
            showProtocols(FrameworkWizardUI.VIEW, intCategory, intLocation);
        }

        //
        //View Hobs
        //
        if (intCategory == IDefaultMutableTreeNode.HOBS_HOB_ITEM) {
            showHobs(FrameworkWizardUI.VIEW, intCategory, intLocation);
        }

        //
        //View Events
        //
        if (intCategory == IDefaultMutableTreeNode.EVENTS_CREATEEVENTS_ITEM
            || intCategory == IDefaultMutableTreeNode.EVENTS_SIGNALEVENTS_ITEM) {
            showEvents(FrameworkWizardUI.VIEW, intCategory, intLocation);
        }

        //
        //View Ppis
        //
        if (intCategory == IDefaultMutableTreeNode.PPIS_PPI_ITEM
            || intCategory == IDefaultMutableTreeNode.PPIS_PPINOTIFY_ITEM) {
            showPpis(FrameworkWizardUI.VIEW, intCategory, intLocation);
        }

        //
        //View Variables
        //
        if (intCategory == IDefaultMutableTreeNode.VARIABLES_VARIABLE_ITEM) {
            showVariables(FrameworkWizardUI.VIEW, intCategory, intLocation);
        }

        //
        //View BootModes
        //
        if (intCategory == IDefaultMutableTreeNode.BOOTMODES_BOOTMODE_ITEM) {
            showBootModes(FrameworkWizardUI.VIEW, intCategory, intLocation);
        }

        //
        //View SystemTables
        //
        if (intCategory == IDefaultMutableTreeNode.SYSTEMTABLES_SYSTEMTABLE_ITEM) {
            showSystemTables(FrameworkWizardUI.VIEW, intCategory, intLocation);
        }

        //
        //View DataHubs
        //
        if (intCategory == IDefaultMutableTreeNode.DATAHUBS_DATAHUB_ITEM) {
            showDataHubs(FrameworkWizardUI.VIEW, intCategory, intLocation);
        }

        //
        //View Formsets
        //
        if (intCategory == IDefaultMutableTreeNode.FORMSETS_FORMSET_ITEM) {
            showFormsets(FrameworkWizardUI.VIEW, intCategory, intLocation);
        }

        //
        //View Guids
        //
        if (intCategory == IDefaultMutableTreeNode.GUIDS_GUIDENTRY_ITEM) {
            showGuids(FrameworkWizardUI.VIEW, intCategory, intLocation);
        }

        //
        //View Externs
        //
        if (intCategory == IDefaultMutableTreeNode.EXTERNS_EXTERN_ITEM) {
            showExterns(FrameworkWizardUI.VIEW, intCategory, intLocation);
        }

        //
        //View PCDs
        //
        if (intCategory == IDefaultMutableTreeNode.PCDS_PCDDATA_ITEM) {
            showPCDs(FrameworkWizardUI.VIEW, intCategory, intLocation);
        }
    }

    /**
     Execute update operation of current module
     
     @param intCategory The category of current node
     @param intLocation The location of current node
     
     **/
    private void updateCurrentModule(int intCategory, int intLocation) {
        //
        //Update Msa Header
        //
        if (intCategory == IDefaultMutableTreeNode.MSA_HEADER) {
            showMsaHeader(FrameworkWizardUI.UPDATE);
        }

        //
        //Update LIBRARY CLASS DEFINITIONS
        //
        if (intCategory == IDefaultMutableTreeNode.LIBRARYCLASSDEFINITIONS) {
            showLibraryClassDefinitions(FrameworkWizardUI.UPDATE, intCategory);
        }

        //
        //Update Source Files
        //
        if (intCategory == IDefaultMutableTreeNode.SOURCEFILES_FILENAME
            || intCategory == IDefaultMutableTreeNode.SOURCEFILES_ARCH_ITEM) {
            showSourceFiles(FrameworkWizardUI.UPDATE, intCategory, intLocation);
        }

        //
        //Update Protocols
        //
        if (intCategory == IDefaultMutableTreeNode.PROTOCOLS_PROTOCOL_ITEM
            || intCategory == IDefaultMutableTreeNode.PROTOCOLS_PROTOCOLNOTIFY_ITEM) {
            showProtocols(FrameworkWizardUI.UPDATE, intCategory, intLocation);
        }

        //
        //Update Hobs
        //
        if (intCategory == IDefaultMutableTreeNode.HOBS_HOB_ITEM) {
            showHobs(FrameworkWizardUI.UPDATE, intCategory, intLocation);
        }

        //
        //Update Events
        //
        if (intCategory == IDefaultMutableTreeNode.EVENTS_CREATEEVENTS_ITEM
            || intCategory == IDefaultMutableTreeNode.EVENTS_SIGNALEVENTS_ITEM) {
            showEvents(FrameworkWizardUI.UPDATE, intCategory, intLocation);
        }

        //
        //Update Ppis
        //
        if (intCategory == IDefaultMutableTreeNode.PPIS_PPI_ITEM
            || intCategory == IDefaultMutableTreeNode.PPIS_PPINOTIFY_ITEM) {
            showPpis(FrameworkWizardUI.UPDATE, intCategory, intLocation);
        }

        //
        //Update Variables
        //
        if (intCategory == IDefaultMutableTreeNode.VARIABLES_VARIABLE_ITEM) {
            showVariables(FrameworkWizardUI.UPDATE, intCategory, intLocation);
        }

        //
        //Update BootModes
        //
        if (intCategory == IDefaultMutableTreeNode.BOOTMODES_BOOTMODE_ITEM) {
            showBootModes(FrameworkWizardUI.UPDATE, intCategory, intLocation);
        }

        //
        //Update SystemTables
        //
        if (intCategory == IDefaultMutableTreeNode.SYSTEMTABLES_SYSTEMTABLE_ITEM) {
            showSystemTables(FrameworkWizardUI.UPDATE, intCategory, intLocation);
        }

        //
        //Update DataHubs
        //
        if (intCategory == IDefaultMutableTreeNode.DATAHUBS_DATAHUB_ITEM) {
            showDataHubs(FrameworkWizardUI.UPDATE, intCategory, intLocation);
        }

        //
        //Update Formsets
        //
        if (intCategory == IDefaultMutableTreeNode.FORMSETS_FORMSET_ITEM) {
            showFormsets(FrameworkWizardUI.UPDATE, intCategory, intLocation);
        }

        //
        //Update Guids
        //
        if (intCategory == IDefaultMutableTreeNode.GUIDS_GUIDENTRY_ITEM) {
            showGuids(FrameworkWizardUI.UPDATE, intCategory, intLocation);
        }

        //
        //Update Externs
        //
        if (intCategory == IDefaultMutableTreeNode.EXTERNS_EXTERN_ITEM) {
            showExterns(FrameworkWizardUI.UPDATE, intCategory, intLocation);
        }

        //
        //Update PCDs
        //
        if (intCategory == IDefaultMutableTreeNode.PCDS_PCDDATA_ITEM) {
            showPCDs(FrameworkWizardUI.UPDATE, intCategory, intLocation);
        }
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
        reloadTreeAndTable(SAVE_WITH_CHANGE);
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
        File f = new File(this.saveFileName);
        ModuleSurfaceAreaDocument msaDoc = ModuleSurfaceAreaDocument.Factory.newInstance();
        ModuleSurfaceAreaDocument.ModuleSurfaceArea msa = ModuleSurfaceAreaDocument.ModuleSurfaceArea.Factory
                                                                                                             .newInstance();

        //
        //Add all components into xml doc file
        //
        if (xmlmh != null) {
            msa.setMsaHeader(xmlmh);
        }
        if (xmllcd != null) {
            msa.setLibraryClassDefinitions(xmllcd);
        }
        if (xmlsf != null) {
            msa.setSourceFiles(xmlsf);
        }
        if (xmlpl != null) {
            msa.setProtocols(xmlpl);
        }
        if (xmlen != null) {
            msa.setEvents(xmlen);
        }
        if (xmlhob != null) {
            msa.setHobs(xmlhob);
        }
        if (xmlppi != null) {
            msa.setPPIs(xmlppi);
        }
        if (xmlvb != null) {
            msa.setVariables(xmlvb);
        }
        if (xmlbm != null) {
            msa.setBootModes(xmlbm);
        }
        if (xmlst != null) {
            msa.setSystemTables(xmlst);
        }
        if (xmldh != null) {
            msa.setDataHubs(xmldh);
        }
        if (xmlfs != null) {
            msa.setFormsets(xmlfs);
        }
        if (xmlgu != null) {
            msa.setGuids(xmlgu);
        }
        if (xmlet != null) {
            msa.setExterns(xmlet);
        }
        if (xmlpcd != null) {
            msa.setPCDs(xmlpcd);
        }
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
        makeTree();
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
            //this.jMenuEditAdd.setEnabled(false);
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

        if (this.currentModuleType == 1 || this.currentModuleType == 3) {
            //this.jMenuItemEditAddLibraries.setEnabled(false);
            //this.jMenuItemEditAddLibraryClassDefinitions.setEnabled(true);
        }
        if (this.currentModuleType == 2 || this.currentModuleType == 4) {
            //this.jMenuItemEditAddLibraries.setEnabled(true);
            //this.jMenuItemEditAddLibraryClassDefinitions.setEnabled(false);
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

    /**
     Show msa header
     When the operation is VIEW, disable all fields of internal frame
     
     @param type The input data of operation type
     
     **/
    private void showMsaHeader(int type) {
        msa = null;
        msa = new MsaHeader(this.xmlmh);
        //        jDesktopPane a = new jDesktopPane();
        //        
        //        a.setBounds(new java.awt.Rectangle(DataType.MAIN_FRAME_EDITOR_PANEL_LOCATION_X, DataType.MAIN_FRAME_EDITOR_PANEL_LOCATION_Y, DataType.MAIN_FRAME_EDITOR_PANEL_PREFERRED_SIZE_WIDTH, DataType.MAIN_FRAME_EDITOR_PANEL_PREFERRED_SIZE_HEIGHT));
        //        a.setMinimumSize(new java.awt.Dimension(DataType.MAIN_FRAME_EDITOR_PANEL_PREFERRED_SIZE_WIDTH, DataType.MAIN_FRAME_EDITOR_PANEL_PREFERRED_SIZE_HEIGHT));
        //        a.setDesktopManager(iDesktopManager);
        //        a.addComponentListener(this);
        //        a.add(msa, 1);
        //jTabbedPaneEditor.addTab("1", null, a, null);

        this.cleanDesktopPane();
        getJDesktopPaneModule().add(msa, 1);
        this.jTabbedPaneEditor.setSelectedIndex(0);
        this.currentNodeType = IDefaultMutableTreeNode.MSA_HEADER;
        this.currentModuleType = 1;
        if (type == FrameworkWizardUI.VIEW) {
            setButtonEnable(false);
            msa.setViewMode(true);
        } else {
            setButtonEnable(true);
        }
    }

    /**
     Show LibraryClassDefinitions
     When the operation is VIEW, disable all fields of internal frame
     
     @param type The input data of operation type
     
     **/
    private void showLibraryClassDefinitions(int operationType, int nodeType) {
        mlcd = null;
        if (operationType == FrameworkWizardUI.ADD) {
            mlcd = new ModuleLibraryClassDefinitions(this.xmllcd);
        }
        if (operationType == FrameworkWizardUI.UPDATE || operationType == FrameworkWizardUI.VIEW) {
            mlcd = new ModuleLibraryClassDefinitions(this.xmllcd);
        }
        this.jDesktopPaneModule.removeAll();
        this.jDesktopPaneModule.add(mlcd, 1);
        this.currentNodeType = nodeType;
        if (operationType == FrameworkWizardUI.VIEW) {
            setButtonEnable(false);
            mlcd.setViewMode(true);
        } else {
            setButtonEnable(true);
        }
    }

    /**
     Show SourceFiles
     When the operation is VIEW, disable all fields of internal frame
     
     @param type The input data of operation type
     
     **/
    private void showSourceFiles(int operationType, int nodeType, int location) {
        msf = null;
        if (operationType == FrameworkWizardUI.ADD) {
            msf = new ModuleSourceFiles(this.xmlsf, -1, -1, 1);
        }
        if (operationType == FrameworkWizardUI.UPDATE || operationType == FrameworkWizardUI.VIEW) {
            msf = new ModuleSourceFiles(this.xmlsf, nodeType, location, 2);
        }
        this.jDesktopPaneModule.removeAll();
        this.jDesktopPaneModule.add(msf, 1);
        this.currentNodeType = nodeType;
        if (operationType == FrameworkWizardUI.VIEW) {
            setButtonEnable(false);
            msf.setViewMode(true);
        } else {
            setButtonEnable(true);
        }
    }

    /**
     Show Protocols
     When the operation is VIEW, disable all fields of internal frame
     
     @param type The input data of operation type
     
     **/
    private void showProtocols(int operationType, int nodeType, int location) {
        mp = null;
        if (operationType == FrameworkWizardUI.ADD) {
            mp = new ModuleProtocols(this.xmlpl);
        }
        if (operationType == FrameworkWizardUI.UPDATE || operationType == FrameworkWizardUI.VIEW) {
            mp = new ModuleProtocols(this.xmlpl, nodeType, location);
        }
        this.jDesktopPaneModule.removeAll();
        this.jDesktopPaneModule.add(mp, 1);
        this.currentNodeType = nodeType;
        if (operationType == FrameworkWizardUI.VIEW) {
            setButtonEnable(false);
            mp.setViewMode(true);
        } else {
            setButtonEnable(true);
        }
    }

    /**
     Show Events
     When the operation is VIEW, disable all fields of internal frame
     
     @param type The input data of operation type
     
     **/
    private void showEvents(int operationType, int nodeType, int location) {
        mev = null;
        if (operationType == FrameworkWizardUI.ADD) {
            mev = new ModuleEvents(this.xmlen);
        }
        if (operationType == FrameworkWizardUI.UPDATE || operationType == FrameworkWizardUI.VIEW) {
            mev = new ModuleEvents(this.xmlen, nodeType, location);
        }
        this.jDesktopPaneModule.removeAll();
        this.jDesktopPaneModule.add(mev, 1);
        this.currentNodeType = nodeType;
        if (operationType == FrameworkWizardUI.VIEW) {
            setButtonEnable(false);
            mev.setViewMode(true);
        } else {
            setButtonEnable(true);
        }
    }

    /**
     Show Hobs
     When the operation is VIEW, disable all fields of internal frame
     
     @param type The input data of operation type
     
     **/
    private void showHobs(int operationType, int nodeType, int location) {
        mh = null;
        if (operationType == FrameworkWizardUI.ADD) {
            mh = new ModuleHobs(this.xmlhob);
        }
        if (operationType == FrameworkWizardUI.UPDATE || operationType == FrameworkWizardUI.VIEW) {
            mh = new ModuleHobs(this.xmlhob, nodeType, location);
        }
        this.jDesktopPaneModule.removeAll();
        this.jDesktopPaneModule.add(mh, 1);
        this.currentNodeType = nodeType;
        if (operationType == FrameworkWizardUI.VIEW) {
            setButtonEnable(false);
            mh.setViewMode(true);
        } else {
            setButtonEnable(true);
        }
    }

    /**
     Show Ppis
     When the operation is VIEW, disable all fields of internal frame
     
     @param type The input data of operation type
     
     **/
    private void showPpis(int operationType, int nodeType, int location) {
        mpp = null;
        if (operationType == FrameworkWizardUI.ADD) {
            mpp = new ModulePpis(this.xmlppi);
        }
        if (operationType == FrameworkWizardUI.UPDATE || operationType == FrameworkWizardUI.VIEW) {
            mpp = new ModulePpis(this.xmlppi, nodeType, location);
        }
        this.jDesktopPaneModule.removeAll();
        this.jDesktopPaneModule.add(mpp, 1);
        this.currentNodeType = nodeType;
        if (operationType == FrameworkWizardUI.VIEW) {
            setButtonEnable(false);
            mpp.setViewMode(true);
        } else {
            setButtonEnable(true);
        }
    }

    /**
     Show Variables
     When the operation is VIEW, disable all fields of internal frame
     
     @param type The input data of operation type
     
     **/
    private void showVariables(int operationType, int nodeType, int location) {
        mv = null;
        if (operationType == FrameworkWizardUI.ADD) {
            mv = new ModuleVariables(this.xmlvb);
        }
        if (operationType == FrameworkWizardUI.UPDATE || operationType == FrameworkWizardUI.VIEW) {
            mv = new ModuleVariables(this.xmlvb, nodeType, location);
        }
        this.jDesktopPaneModule.removeAll();
        this.jDesktopPaneModule.add(mv, 1);
        this.currentNodeType = nodeType;
        if (operationType == FrameworkWizardUI.VIEW) {
            setButtonEnable(false);
            mv.setViewMode(true);
        } else {
            setButtonEnable(true);
        }
    }

    /**
     Show BootModes
     When the operation is VIEW, disable all fields of internal frame
     
     @param type The input data of operation type
     
     **/
    private void showBootModes(int operationType, int nodeType, int location) {
        mbm = null;
        if (operationType == FrameworkWizardUI.ADD) {
            mbm = new ModuleBootModes(this.xmlbm);
        }
        if (operationType == FrameworkWizardUI.UPDATE || operationType == FrameworkWizardUI.VIEW) {
            mbm = new ModuleBootModes(this.xmlbm, nodeType, location);
        }
        this.jDesktopPaneModule.removeAll();
        this.jDesktopPaneModule.add(mbm, 1);
        this.currentNodeType = nodeType;
        if (operationType == FrameworkWizardUI.VIEW) {
            setButtonEnable(false);
            mbm.setViewMode(true);
        } else {
            setButtonEnable(true);
        }
    }

    /**
     Show SystemTables
     When the operation is VIEW, disable all fields of internal frame
     
     @param type The input data of operation type
     
     **/
    private void showSystemTables(int operationType, int nodeType, int location) {
        mst = null;
        if (operationType == FrameworkWizardUI.ADD) {
            mst = new ModuleSystemTables(this.xmlst);
        }
        if (operationType == FrameworkWizardUI.UPDATE || operationType == FrameworkWizardUI.VIEW) {
            mst = new ModuleSystemTables(this.xmlst, nodeType, location);
        }
        this.jDesktopPaneModule.removeAll();
        this.jDesktopPaneModule.add(mst, 1);
        this.currentNodeType = nodeType;
        if (operationType == FrameworkWizardUI.VIEW) {
            setButtonEnable(false);
            mst.setViewMode(true);
        } else {
            setButtonEnable(true);
        }
    }

    /**
     Show DataHubs
     When the operation is VIEW, disable all fields of internal frame
     
     @param type The input data of operation type
     
     **/
    private void showDataHubs(int operationType, int nodeType, int location) {
        mdh = null;
        if (operationType == FrameworkWizardUI.ADD) {
            mdh = new ModuleDataHubs(this.xmldh);
        }
        if (operationType == FrameworkWizardUI.UPDATE || operationType == FrameworkWizardUI.VIEW) {
            mdh = new ModuleDataHubs(this.xmldh, nodeType, location);
        }
        this.jDesktopPaneModule.removeAll();
        this.jDesktopPaneModule.add(mdh, 1);
        this.currentNodeType = nodeType;
        if (operationType == FrameworkWizardUI.VIEW) {
            setButtonEnable(false);
            mdh.setViewMode(true);
        } else {
            setButtonEnable(true);
        }
    }

    /**
     Show Formsets
     When the operation is VIEW, disable all fields of internal frame
     
     @param type The input data of operation type
     
     **/
    private void showFormsets(int operationType, int nodeType, int location) {
        mf = null;
        if (operationType == FrameworkWizardUI.ADD) {
            mf = new ModuleFormsets(this.xmlfs);
        }
        if (operationType == FrameworkWizardUI.UPDATE || operationType == FrameworkWizardUI.VIEW) {
            mf = new ModuleFormsets(this.xmlfs, nodeType, location);
        }
        this.jDesktopPaneModule.removeAll();
        this.jDesktopPaneModule.add(mf, 1);
        this.currentNodeType = nodeType;
        if (operationType == FrameworkWizardUI.VIEW) {
            setButtonEnable(false);
            mf.setViewMode(true);
        } else {
            setButtonEnable(true);
        }
    }

    /**
     Show Show Guids
     When the operation is VIEW, disable all fields of internal frame
     
     @param type The input data of operation type
     
     **/
    private void showGuids(int operationType, int nodeType, int location) {
        mg = null;
        if (operationType == FrameworkWizardUI.ADD || operationType == FrameworkWizardUI.VIEW) {
            mg = new ModuleGuids(this.xmlgu);
        }
        if (operationType == FrameworkWizardUI.UPDATE || operationType == FrameworkWizardUI.VIEW) {
            mg = new ModuleGuids(this.xmlgu, nodeType, location);
        }
        this.jDesktopPaneModule.removeAll();
        this.jDesktopPaneModule.add(mg, 1);
        this.currentNodeType = nodeType;
        if (operationType == FrameworkWizardUI.VIEW) {
            setButtonEnable(false);
            mg.setViewMode(true);
        } else {
            setButtonEnable(true);
        }
    }

    /**
     Show Externs
     When the operation is VIEW, disable all fields of internal frame
     
     @param type The input data of operation type
     
     **/
    private void showExterns(int operationType, int nodeType, int location) {
        met = null;
        if (operationType == FrameworkWizardUI.ADD) {
            met = new ModuleExterns(this.xmlet);
        }
        if (operationType == FrameworkWizardUI.UPDATE || operationType == FrameworkWizardUI.VIEW) {
            met = new ModuleExterns(this.xmlet, nodeType, location);
        }
        this.jDesktopPaneModule.removeAll();
        this.jDesktopPaneModule.add(met, 1);
        this.currentNodeType = nodeType;
        if (operationType == FrameworkWizardUI.VIEW) {
            setButtonEnable(false);
            met.setViewMode(true);
        } else {
            setButtonEnable(true);
        }
    }

    /**
     Show PCDs
     When the operation is VIEW, disable all fields of internal frame
     
     @param type The input data of operation type
     
     **/
    private void showPCDs(int operationType, int nodeType, int location) {
        mpcd = null;
        if (operationType == FrameworkWizardUI.ADD) {
            mpcd = new ModulePCDs(this.xmlpcd);
        }
        if (operationType == FrameworkWizardUI.UPDATE || operationType == FrameworkWizardUI.VIEW) {
            mpcd = new ModulePCDs(this.xmlpcd, nodeType, location);
        }
        this.jDesktopPaneModule.removeAll();
        this.jDesktopPaneModule.add(mpcd, 1);
        this.currentNodeType = nodeType;
        if (operationType == FrameworkWizardUI.VIEW) {
            setButtonEnable(false);
            mpcd.setViewMode(true);
        } else {
            setButtonEnable(true);
        }
    }

    /**
     Save currentModule when press button OK
     
     **/
    private void save() {
        if (this.currentNodeType == IDefaultMutableTreeNode.MSA_HEADER) {
            if (!msa.check()) {
                return;
            }
            msa.save();
            msa.setViewMode(true);
            this.xmlmh = msa.getMsaHeader();
        }

        if (this.currentNodeType == IDefaultMutableTreeNode.LIBRARYCLASSDEFINITIONS
            || this.currentNodeType == IDefaultMutableTreeNode.LIBRARY_CLASS_DEFINITION) {
            if (!mlcd.check()) {
                return;
            }
            mlcd.save();
            mlcd.setViewMode(true);
            this.xmllcd = mlcd.getLibraryClassDefinitions();
        }

        if (this.currentNodeType == IDefaultMutableTreeNode.SOURCEFILES
            || this.currentNodeType == IDefaultMutableTreeNode.SOURCEFILES_ARCH
            || this.currentNodeType == IDefaultMutableTreeNode.SOURCEFILES_ARCH_ITEM
            || this.currentNodeType == IDefaultMutableTreeNode.SOURCEFILES_FILENAME
            || this.currentNodeType == IDefaultMutableTreeNode.SOURCEFILES_FILENAME_ITEM) {
            if (!msf.check()) {
                return;
            }
            msf.save();
            msf.setViewMode(true);
            this.xmlsf = msf.getSourceFiles();
        }

        if (this.currentNodeType == IDefaultMutableTreeNode.PROTOCOLS
            || this.currentNodeType == IDefaultMutableTreeNode.PROTOCOLS_PROTOCOL
            || this.currentNodeType == IDefaultMutableTreeNode.PROTOCOLS_PROTOCOL_ITEM
            || this.currentNodeType == IDefaultMutableTreeNode.PROTOCOLS_PROTOCOLNOTIFY
            || this.currentNodeType == IDefaultMutableTreeNode.PROTOCOLS_PROTOCOLNOTIFY_ITEM) {
            if (!mp.check()) {
                return;
            }
            mp.save();
            mp.setViewMode(true);
            this.xmlpl = mp.getProtocols();
        }

        if (this.currentNodeType == IDefaultMutableTreeNode.EVENTS
            || this.currentNodeType == IDefaultMutableTreeNode.EVENTS_CREATEEVENTS
            || this.currentNodeType == IDefaultMutableTreeNode.EVENTS_CREATEEVENTS_ITEM
            || this.currentNodeType == IDefaultMutableTreeNode.EVENTS_SIGNALEVENTS
            || this.currentNodeType == IDefaultMutableTreeNode.EVENTS_SIGNALEVENTS_ITEM) {
            if (!mev.check()) {
                return;
            }
            mev.save();
            mev.setViewMode(true);
            this.xmlen = mev.getEvents();
        }

        if (this.currentNodeType == IDefaultMutableTreeNode.HOBS
            || this.currentNodeType == IDefaultMutableTreeNode.HOBS_HOB_ITEM) {
            if (!mh.check()) {
                return;
            }
            mh.save();
            mh.setViewMode(true);
            this.xmlhob = mh.getHobs();
        }

        if (this.currentNodeType == IDefaultMutableTreeNode.PPIS
            || this.currentNodeType == IDefaultMutableTreeNode.PPIS_PPI
            || this.currentNodeType == IDefaultMutableTreeNode.PPIS_PPI_ITEM
            || this.currentNodeType == IDefaultMutableTreeNode.PPIS_PPINOTIFY
            || this.currentNodeType == IDefaultMutableTreeNode.PPIS_PPINOTIFY_ITEM) {
            if (!mpp.check()) {
                return;
            }
            mpp.save();
            mpp.setViewMode(true);
            this.xmlppi = mpp.getPpis();
        }

        if (this.currentNodeType == IDefaultMutableTreeNode.VARIABLES
            || this.currentNodeType == IDefaultMutableTreeNode.VARIABLES_VARIABLE_ITEM) {
            if (!mv.check()) {
                return;
            }
            mv.save();
            mv.setViewMode(true);
            this.xmlvb = mv.getVariables();
        }

        if (this.currentNodeType == IDefaultMutableTreeNode.BOOTMODES
            || this.currentNodeType == IDefaultMutableTreeNode.BOOTMODES_BOOTMODE_ITEM) {
            if (!mbm.check()) {
                return;
            }
            mbm.save();
            mbm.setViewMode(true);
            this.xmlbm = mbm.getBootModes();
        }

        if (this.currentNodeType == IDefaultMutableTreeNode.SYSTEMTABLES
            || this.currentNodeType == IDefaultMutableTreeNode.SYSTEMTABLES_SYSTEMTABLE_ITEM) {
            if (!mst.check()) {
                return;
            }
            mst.save();
            mst.setViewMode(true);
            this.xmlst = mst.getSystemTables();
        }

        if (this.currentNodeType == IDefaultMutableTreeNode.DATAHUBS
            || this.currentNodeType == IDefaultMutableTreeNode.DATAHUBS_DATAHUB_ITEM) {
            if (!mdh.check()) {
                return;
            }
            mdh.save();
            mdh.setViewMode(true);
            this.xmldh = mdh.getDataHubs();
        }

        if (this.currentNodeType == IDefaultMutableTreeNode.FORMSETS
            || this.currentNodeType == IDefaultMutableTreeNode.FORMSETS_FORMSET_ITEM) {
            if (!mf.check()) {
                return;
            }
            mf.save();
            mf.setViewMode(true);
            this.xmlfs = mf.getFormsets();
        }

        if (this.currentNodeType == IDefaultMutableTreeNode.GUIDS
            || this.currentNodeType == IDefaultMutableTreeNode.GUIDS_GUIDENTRY_ITEM) {
            if (!mg.check()) {
                return;
            }
            mg.save();
            mg.setViewMode(true);
            this.xmlgu = mg.getGuids();
        }

        if (this.currentNodeType == IDefaultMutableTreeNode.EXTERNS
            || this.currentNodeType == IDefaultMutableTreeNode.EXTERNS_EXTERN_ITEM) {
            if (!met.check()) {
                return;
            }
            met.save();
            met.setViewMode(true);
            this.xmlet = met.getExterns();
        }

        if (this.currentNodeType == IDefaultMutableTreeNode.PCDS
            || this.currentNodeType == IDefaultMutableTreeNode.PCDS_PCDDATA_ITEM) {
            if (!mpcd.check()) {
                return;
            }
            mpcd.save();
            mpcd.setViewMode(true);
            this.xmlpcd = mpcd.getPcds();
        }

        reloadTreeAndTable(NEW_WITH_CHANGE);
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

    private void insertModuleTreeNode() {
        iTree.addNode(new IDefaultMutableTreeNode("Source Files", IDefaultMutableTreeNode.SOURCEFILES, -1));
        iTree.addNode(new IDefaultMutableTreeNode("Library Class Definitions",
                                                  IDefaultMutableTreeNode.LIBRARYCLASSDEFINITIONS, -1));
        iTree.addNode(new IDefaultMutableTreeNode("Includes", IDefaultMutableTreeNode.INCLUDES, -1));
        iTree.addNode(new IDefaultMutableTreeNode("Boot Modes", IDefaultMutableTreeNode.BOOTMODES, -1));
        iTree.addNode(new IDefaultMutableTreeNode("Data Hubs", IDefaultMutableTreeNode.DATAHUBS, -1));
        iTree.addNode(new IDefaultMutableTreeNode("Events", IDefaultMutableTreeNode.EVENTS, -1));
        iTree.addNode(new IDefaultMutableTreeNode("Externs", IDefaultMutableTreeNode.EXTERNS, -1));
        iTree.addNode(new IDefaultMutableTreeNode("Formats", IDefaultMutableTreeNode.FORMSETS, -1));
        iTree.addNode(new IDefaultMutableTreeNode("Guids", IDefaultMutableTreeNode.GUIDS, -1));
        iTree.addNode(new IDefaultMutableTreeNode("Hobs", IDefaultMutableTreeNode.HOBS, -1));
        iTree.addNode(new IDefaultMutableTreeNode("PCDs", IDefaultMutableTreeNode.PCDS, -1));
        iTree.addNode(new IDefaultMutableTreeNode("Ppis", IDefaultMutableTreeNode.PPIS, -1));
        iTree.addNode(new IDefaultMutableTreeNode("Protocols", IDefaultMutableTreeNode.PROTOCOLS, -1));
        iTree.addNode(new IDefaultMutableTreeNode("System Tables", IDefaultMutableTreeNode.SYSTEMTABLES, -1));
        iTree.addNode(new IDefaultMutableTreeNode("Variables", IDefaultMutableTreeNode.VARIABLES, -1));
    }

    private void insertPackageTreeNode() {

    }

    private void insertPlatformTreeNode() {

    }

    /**
     Operation when double click a tree node
     
     **/
    private void doubleClickModuleTreeNode() {
        if (!iTree.getSelectNode().isOpening()) {
            //          
            // If the node is not opened yet
            // Insert top level elements first
            //
            if (iTree.getSelectCategory() == IDefaultMutableTreeNode.MODULE) {
                //openMsaFile(iTree.getSelectNode().getId().getPath());
                String strMsaFilePath = iTree.getSelectNode().getId().getName(); 
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
                insertModuleTreeNode();
                iTree.getSelectNode().setOpening(true);
            }
            if (iTree.getSelectCategory() == IDefaultMutableTreeNode.PACKAGE) {
                insertPackageTreeNode();
            }
            if (iTree.getSelectCategory() == IDefaultMutableTreeNode.PLATFORM) {
                insertPlatformTreeNode();
            }
        } else {
            //          
            // If the node is opened already
            // Just show it
            //
            if (iTree.getSelectCategory() == IDefaultMutableTreeNode.MODULE) {
            }
            
            if (iTree.getSelectCategory() == IDefaultMutableTreeNode.PACKAGE) {
            
            }
            if (iTree.getSelectCategory() == IDefaultMutableTreeNode.PLATFORM) {
            
            }
        }
    }
    
    /**
    Close all opening files and clean all showing internal frame
    
    **/
    private void closeAll() {
        this.cleanDesktopPane();
        this.makeEmptyTree();
    }
}
