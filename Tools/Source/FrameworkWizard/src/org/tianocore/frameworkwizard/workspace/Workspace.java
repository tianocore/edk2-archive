/** @file
 
 The file is used to init workspace and get basic information of workspace
 
 Copyright (c) 2006, Intel Corporation
 All rights reserved. This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution.  The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php
 
 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 
 **/

package org.tianocore.frameworkwizard.workspace;

import java.io.File;
import java.util.Vector;

import org.apache.xmlbeans.XmlException;
import org.apache.xmlbeans.XmlObject;
import org.tianocore.FrameworkDatabaseDocument;
import org.tianocore.PackageSurfaceAreaDocument;
import org.tianocore.frameworkwizard.common.Log;
import org.tianocore.frameworkwizard.common.Tools;
import org.tianocore.frameworkwizard.module.ModuleIdentification;
import org.tianocore.frameworkwizard.packaging.PackageIdentification;
import org.tianocore.frameworkwizard.platform.PlatformIdentification;

/**
 The file is used to init workspace and get basic information of workspace
 
 @since ModuleEditor 1.0
 
 */
public class Workspace {

    //
    // Define class members
    //
    private String currentWorkspace = null;

    private FrameworkDatabaseDocument xmlFrameworkDbDoc = null;

    private PackageSurfaceAreaDocument xmlPackageSpdDoc = null;

    private Vector<ModuleIdentification> vModuleList = new Vector<ModuleIdentification>();

    private Vector<PackageIdentification> vPackageList = new Vector<PackageIdentification>();

    private Vector<PlatformIdentification> vPlatformList = new Vector<PlatformIdentification>();

    private String strWorkspaceDatabaseFile = Tools.FIEL_SEPARATOR + "Tools" + Tools.FIEL_SEPARATOR + "Conf"
                                              + Tools.FIEL_SEPARATOR + "FrameworkDatabase.db";

    public static void main(String[] args) {

    }

    /**
     This is the default constructor Get current WORKSPACE from system
     environment variable
     
     */
    public Workspace() {
        this.currentWorkspace = System.getenv("WORKSPACE");
    }

    /**
     Check if current workspace exists of not
     
     @retval true - The current WORKSPACE exists
     @retval false - The current WORKSPACE doesn't exist
     
     */
    public boolean checkCurrentWorkspace() {
        return checkCurrentWorkspace(getCurrentWorkspace());
    }

    /**
     Check if current workspace exists or not via input workspace path
     
     @param strWorkspace
     The input data of WORKSPACE path
     @retval true - The current WORKSPACE exists
     @retval false - The current WORKSPACE doesn't exist
     
     */
    public boolean checkCurrentWorkspace(String strWorkspace) {
        if (strWorkspace == null || strWorkspace == "") {
            return false;
        }
        File f = new File(strWorkspace);
        if (!f.isDirectory()) {
            return false;
        }
        if (!f.exists()) {
            return false;
        }
        return true;
    }

    /**
     Get Current Workspace
     
     @return currentWorkspace
     
     */
    public String getCurrentWorkspace() {
        return currentWorkspace;
    }

    /**
     Set Current Workspace
     
     @param currentWorkspace
     The input data of currentWorkspace
     
     */
    public void setCurrentWorkspace(String currentWorkspace) {
        this.currentWorkspace = currentWorkspace;
    }

    /**
     
     Open Framework Database file
     
     */
    private void openFrameworkDb() {
        String strFrameworkDbFilePath = this.getCurrentWorkspace() + strWorkspaceDatabaseFile;
        File db = new File(strFrameworkDbFilePath);
        try {
            xmlFrameworkDbDoc = (FrameworkDatabaseDocument) XmlObject.Factory.parse(db);
        } catch (XmlException e) {
            Log.err("Open Framework Database " + strFrameworkDbFilePath, e.getMessage());
            return;
        } catch (Exception e) {
            Log.err("Open Framework Database " + strFrameworkDbFilePath, "Invalid file type");
            return;
        }
    }

    /**
     
     Open package file
     
     */
    private void openPackageSpdFile(String name) {
        String strFileName = this.getCurrentWorkspace() + System.getProperty("file.separator") + name;
        File spdFile = new File(strFileName);
        try {
            xmlPackageSpdDoc = (PackageSurfaceAreaDocument) XmlObject.Factory.parse(spdFile);
        } catch (XmlException e) {
            Log.err("Open Pacakce File " + spdFile, e.getMessage());
            return;
        } catch (Exception e) {
            Log.err("Open Package File " + spdFile, "Invalid file type");
            return;
        }
    }

    /**
     Get FrameworkDatabaseDocument
     
     @return FrameworkDatabaseDocument
     
     */
    public FrameworkDatabaseDocument getXmlFrameworkDbDoc() {
        openFrameworkDb();
        return xmlFrameworkDbDoc;
    }

    /**
     Set FrameworkDatabaseDocument
     
     @param xmlFrameworkDbDoc
     The input data of FrameworkDatabaseDocument
     
     */
    public void setXmlFrameworkDbDoc(FrameworkDatabaseDocument xmlFrameworkDbDoc) {
        this.xmlFrameworkDbDoc = xmlFrameworkDbDoc;
    }

    /**
     Get all package basic information form the FrameworkDatabase.db file
     
     @return vPackageList A vector includes all packages' basic information
     
     */
    public Vector<PackageIdentification> getAllPackages() {
        String name = "";
        String guid = "";
        String version = "";
        String path = "";

        openFrameworkDb();

        for (int index = 0; index < xmlFrameworkDbDoc.getFrameworkDatabase().getPackageList().sizeOfPackageArray(); index++) {
            name = xmlFrameworkDbDoc.getFrameworkDatabase().getPackageList().getPackageArray(index).getPackageName();
            guid = xmlFrameworkDbDoc.getFrameworkDatabase().getPackageList().getPackageArray(index).getPackageGuid();
            version = xmlFrameworkDbDoc.getFrameworkDatabase().getPackageList().getPackageArray(index)
                                       .getPackageVersion().toString();
            path = this.getCurrentWorkspace()
                   + Tools.FIEL_SEPARATOR
                   + xmlFrameworkDbDoc.getFrameworkDatabase().getPackageList().getPackageArray(index).getFileList()
                                      .get(0);
            vPackageList.addElement(new PackageIdentification(name, guid, version, path));
        }

        return vPackageList;
    }

    /**
     Get all package basic information form the FrameworkDatabase.db file
     
     @return vPackageList A vector includes all packages' basic information
     
     */
    public Vector<ModuleIdentification> getAllModules() {
        PackageIdentification packageID = null;
        String name = "";
        String guid = "";
        String version = "";
        String packagePath = "";
        String modulePath = "";

        openFrameworkDb();

        for (int index = 0; index < xmlFrameworkDbDoc.getFrameworkDatabase().getPackageList().sizeOfPackageArray(); index++) {
            openPackageSpdFile(xmlFrameworkDbDoc.getFrameworkDatabase().getPackageList().getPackageArray(index)
                                                .getFileList().get(0));
            name = xmlFrameworkDbDoc.getFrameworkDatabase().getPackageList().getPackageArray(index).getPackageName();
            guid = xmlFrameworkDbDoc.getFrameworkDatabase().getPackageList().getPackageArray(index).getPackageGuid();
            version = xmlFrameworkDbDoc.getFrameworkDatabase().getPackageList().getPackageArray(index)
                                       .getPackageVersion().toString();
            packagePath = xmlFrameworkDbDoc.getFrameworkDatabase().getPackageList().getPackageArray(index)
                                           .getFileList().get(0);
            packagePath = packagePath.substring(0, packagePath.lastIndexOf("/"));
            packagePath = this.getCurrentWorkspace() + Tools.FIEL_SEPARATOR + packagePath + Tools.FIEL_SEPARATOR;
            packageID = new PackageIdentification(name, guid, version, packagePath);
            for (int indexJ = 0; indexJ < xmlPackageSpdDoc.getPackageSurfaceArea().getMsaFiles().getMsaFileList()
                                                          .size(); indexJ++) {
                name = xmlPackageSpdDoc.getPackageSurfaceArea().getMsaFiles().getMsaFileList().get(indexJ)
                                       .getModuleName();
                guid = xmlPackageSpdDoc.getPackageSurfaceArea().getMsaFiles().getMsaFileList().get(indexJ)
                                       .getModuleGuid();
                //version = xmlPackageSpdDoc.getPackageSurfaceArea().getMsaFiles().getMsaFileList().get(indexJ).getModuleVersion().toString();
                modulePath = packagePath
                             + xmlPackageSpdDoc.getPackageSurfaceArea().getMsaFiles().getMsaFileList().get(indexJ)
                                               .getFilename();
                vModuleList.addElement(new ModuleIdentification(modulePath, guid, version, modulePath, packageID));
            }
        }

        return vModuleList;
    }

    /**
     Get all platform basic information form the FrameworkDatabase.db file
     
     @return vplatformList A vector includes all platforms' basic information
     
     */
    public Vector<PlatformIdentification> getAllPlatforms() {
        String name = "";
        String guid = "";
        String version = "";
        String path = "";

        openFrameworkDb();

        for (int index = 0; index < xmlFrameworkDbDoc.getFrameworkDatabase().getPlatformList().sizeOfPlatformArray(); index++) {
            name = xmlFrameworkDbDoc.getFrameworkDatabase().getPlatformList().getPlatformArray(index).getPlatformName();
            guid = xmlFrameworkDbDoc.getFrameworkDatabase().getPlatformList().getPlatformArray(index).getPlatformGuid();
            version = xmlFrameworkDbDoc.getFrameworkDatabase().getPlatformList().getPlatformArray(index)
                                       .getPlatformVersion().toString();
            path = this.getCurrentWorkspace() + Tools.FIEL_SEPARATOR
                   + xmlFrameworkDbDoc.getFrameworkDatabase().getPlatformList().getPlatformArray(index).getFile();
            vPlatformList.addElement(new PlatformIdentification(name, guid, version, path));
        }

        return vPlatformList;
    }
}
