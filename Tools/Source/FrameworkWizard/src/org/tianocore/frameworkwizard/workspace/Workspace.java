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
import org.tianocore.frameworkwizard.common.Log;
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

	private Vector<ModuleIdentification> vModuleList = new Vector<ModuleIdentification>();

	private Vector<PackageIdentification> vPackageList = new Vector<PackageIdentification>();

	private Vector<PlatformIdentification> vPlatformList = new Vector<PlatformIdentification>();

	private String strWorkspaceDatabaseFile = System
			.getProperty("file.separator")
			+ "Tools"
			+ System.getProperty("file.separator")
			+ "Conf"
			+ System.getProperty("file.separator") + "FrameworkDatabase.db";

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
		String strFrameworkDbFilePath = this.getCurrentWorkspace()
				+ strWorkspaceDatabaseFile;
		File db = new File(strFrameworkDbFilePath);
		try {
			xmlFrameworkDbDoc = (FrameworkDatabaseDocument) XmlObject.Factory
					.parse(db);
		} catch (XmlException e) {
			Log.err("Open Framework Database " + strFrameworkDbFilePath, e
					.getMessage());
			return;
		} catch (Exception e) {
			Log.err("Open Framework Database " + strFrameworkDbFilePath,
					"Invalid file type");
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
	public Vector getAllPackages() {
		String name = "";
		String guid = "";
		String version = "";
		String path = "";

		openFrameworkDb();

		for (int index = 0; index < xmlFrameworkDbDoc.getFrameworkDatabase()
				.getPackageList().sizeOfPackageArray(); index++) {
			name = xmlFrameworkDbDoc.getFrameworkDatabase().getPackageList()
					.getPackageArray(index).getPackageName();
			guid = xmlFrameworkDbDoc.getFrameworkDatabase().getPackageList()
					.getPackageArray(index).getPackageGuid();
			version = xmlFrameworkDbDoc.getFrameworkDatabase().getPackageList()
					.getPackageArray(index).getPackageVersion().toString();
			path = xmlFrameworkDbDoc.getFrameworkDatabase().getPackageList()
					.getPackageArray(index).getFileList().get(0);
			vPackageList.addElement(new PackageIdentification(name, guid,
					version, path));
		}

		return vPackageList;
	}

	/**
	  Get all package basic information form the FrameworkDatabase.db file
	  
	  @return vPackageList A vector includes all packages' basic information
	  
	 */
	public Vector getAllModules() {
		String name = "";
		String guid = "";
		String version = "";
		String path = "";

		openFrameworkDb();

		for (int index = 0; index < xmlFrameworkDbDoc.getFrameworkDatabase()
				.getPackageList().sizeOfPackageArray(); index++) {
			name = xmlFrameworkDbDoc.getFrameworkDatabase().getPackageList()
					.getPackageArray(index).getPackageName();
			guid = xmlFrameworkDbDoc.getFrameworkDatabase().getPackageList()
					.getPackageArray(index).getPackageGuid();
			version = xmlFrameworkDbDoc.getFrameworkDatabase().getPackageList()
					.getPackageArray(index).getPackageVersion().toString();
			vModuleList.addElement(new ModuleIdentification(name, guid,
					version, path));
		}

		return vModuleList;
	}

	/**
	  Get all platform basic information form the FrameworkDatabase.db file
	  
	  @return vplatformList A vector includes all platforms' basic information
	  
	 */
	public Vector getAllPlatforms() {
		String name = "";
		String guid = "";
		String version = "";
		String path = "";

		openFrameworkDb();

		for (int index = 0; index < xmlFrameworkDbDoc.getFrameworkDatabase()
				.getPlatformList().sizeOfPlatformArray(); index++) {
			name = xmlFrameworkDbDoc.getFrameworkDatabase().getPlatformList()
					.getPlatformArray(index).getPlatformName();
			guid = xmlFrameworkDbDoc.getFrameworkDatabase().getPlatformList()
					.getPlatformArray(index).getPlatformGuid();
			version = xmlFrameworkDbDoc.getFrameworkDatabase()
					.getPlatformList().getPlatformArray(index)
					.getPlatformVersion().toString();
			path = xmlFrameworkDbDoc.getFrameworkDatabase().getPlatformList()
					.getPlatformArray(index).getFile();
			vPlatformList.addElement(new PlatformIdentification(name, guid,
					version, path));
		}

		return vPlatformList;
	}
}
