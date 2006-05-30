/** @file
 
 The file is used to define opening file list
 
 Copyright (c) 2006, Intel Corporation
 All rights reserved. This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution.  The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php
 
 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 
 **/

package org.tianocore.frameworkwizard.common;

import java.util.Vector;

import org.tianocore.FrameworkPlatformDescriptionDocument;
import org.tianocore.ModuleSurfaceAreaDocument;
import org.tianocore.PackageSurfaceAreaDocument;

public class OpeningFileList {

    private Vector<OpeningFileIdentification> vOpeningModuleList = new Vector<OpeningFileIdentification>();

    private Vector<OpeningFileIdentification> vOpeningPackageList = new Vector<OpeningFileIdentification>();

    private Vector<OpeningFileIdentification> vOpeningPlatformList = new Vector<OpeningFileIdentification>();
    
    public OpeningFileList() {
        
    }
    
    public Vector<OpeningFileIdentification> getVOpeningModuleList() {
        return vOpeningModuleList;
    }

    public void setVOpeningModuleList(Vector<OpeningFileIdentification> openingModuleList) {
        vOpeningModuleList = openingModuleList;
    }

    public Vector<OpeningFileIdentification> getVOpeningPackageList() {
        return vOpeningPackageList;
    }

    public void setVOpeningPackageList(Vector<OpeningFileIdentification> openingPackageList) {
        vOpeningPackageList = openingPackageList;
    }

    public Vector<OpeningFileIdentification> getVOpeningPlatformList() {
        return vOpeningPlatformList;
    }

    public void setVOpeningPlatformList(Vector<OpeningFileIdentification> openingPlatformList) {
        vOpeningPlatformList = openingPlatformList;
    }
    
    public void insertToOpeningModuleList(Identification id, ModuleSurfaceAreaDocument.ModuleSurfaceArea xmlMsa) {
        vOpeningModuleList.addElement(new OpeningFileIdentification(id, xmlMsa));
    }
    
    public void insertToOpeningPackageList(Identification id, PackageSurfaceAreaDocument.PackageSurfaceArea xmlSpd) {
        vOpeningPackageList.addElement(new OpeningFileIdentification(id, xmlSpd));
    }
    
    public void insertToOpeningPlatformList(Identification id, FrameworkPlatformDescriptionDocument.FrameworkPlatformDescription xmlFpd) {
        vOpeningPlatformList.addElement(new OpeningFileIdentification(id, xmlFpd));
    }
    
    public void removeFromOpeningModuleList(Identification id) {
        for (int index = 0; index < vOpeningModuleList.size(); index++) {
            if (vOpeningModuleList.elementAt(index).getModuleID().equals(id)) {
                vOpeningModuleList.removeElementAt(index);
                break;
            }
        }
    }
    
    public void removeFromOpeningPackageList(Identification id) {
        for (int index = 0; index < vOpeningPackageList.size(); index++) {
            if (vOpeningPackageList.elementAt(index).getPacakgeID().equals(id)) {
                vOpeningPackageList.removeElementAt(index);
                break;
            }
        }
    }
    
    public void removeFromOpeningPlatformList(Identification id) {
        for (int index = 0; index < vOpeningPlatformList.size(); index++) {
            if (vOpeningPlatformList.elementAt(index).getPlatformID().equals(id)) {
                vOpeningPlatformList.removeElementAt(index);
                break;
            }
        }
    }
    
    public void removeAllFromOpeningModuleList() {
        vOpeningModuleList.removeAllElements();
    }

    public void removeAllFromOpeningPackageList() {
        vOpeningPackageList.removeAllElements();
    }
    
    public void removeAllFromOpeningPlatformList() {
        vOpeningPlatformList.removeAllElements();
    }
    
    
    
    public ModuleSurfaceAreaDocument.ModuleSurfaceArea getModuleSurfaceAreaFromId(Identification id) {
        for (int index = 0; index < vOpeningModuleList.size(); index++) {
            if (vOpeningModuleList.elementAt(index).getModuleID().equals(id)) {
                return vOpeningModuleList.elementAt(index).getXmlMsa();
            }
        }
        return null;
    }
    
    public PackageSurfaceAreaDocument.PackageSurfaceArea getPackageSurfaceAreaFromId(Identification id) {
        for (int index = 0; index < vOpeningPackageList.size(); index++) {
            if (vOpeningPackageList.elementAt(index).getPacakgeID().equals(id)) {
                return vOpeningPackageList.elementAt(index).getXmlSpd();
            }
        }
        return null;
    }
    
    public FrameworkPlatformDescriptionDocument.FrameworkPlatformDescription getFrameworkPlatformDescriptionFromId(Identification id) {
        for (int index = 0; index < vOpeningPlatformList.size(); index++) {
            if (vOpeningPlatformList.elementAt(index).getPacakgeID().equals(id)) {
                return vOpeningPlatformList.elementAt(index).getXmlFpd();
            }
        }
        return null;
    }
    
    public boolean existsModule(Identification id) {
        for (int index = 0; index < vOpeningModuleList.size(); index++) {
            if (vOpeningModuleList.elementAt(index).getModuleID().equals(id)) {
                return true;
            }
        }
        return false;
    }
    
    public boolean existsPackage(Identification id) {
        for (int index = 0; index < vOpeningPackageList.size(); index++) {
            if (vOpeningPackageList.elementAt(index).getPacakgeID().equals(id)) {
                return true;
            }
        }
        return false;
    }
    
    public boolean existsPlatform(Identification id) {
        for (int index = 0; index < vOpeningPlatformList.size(); index++) {
            if (vOpeningPlatformList.elementAt(index).getPlatformID().equals(id)) {
                return true;
            }
        }
        return false;
    }
    
    public void removeAll() {
        removeAllFromOpeningModuleList();
        removeAllFromOpeningPackageList();
        removeAllFromOpeningPlatformList();
    }
}
