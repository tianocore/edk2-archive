/** @file
 
 The file is used to define opening file
 
 Copyright (c) 2006, Intel Corporation
 All rights reserved. This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution.  The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php
 
 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 
 **/

package org.tianocore.frameworkwizard.common;

import org.tianocore.FrameworkPlatformDescriptionDocument;
import org.tianocore.ModuleSurfaceAreaDocument;
import org.tianocore.PackageSurfaceAreaDocument;

public class OpeningFileIdentification {
    ///
    /// Define class members
    ///
    private Identification moduleID = null;
    
    private Identification pacakgeID = null;
    
    private Identification platformID = null;
    
    private ModuleSurfaceAreaDocument.ModuleSurfaceArea xmlMsa = null;
    
    private PackageSurfaceAreaDocument.PackageSurfaceArea xmlSpd = null;
    
    private FrameworkPlatformDescriptionDocument.FrameworkPlatformDescription xmlFpd = null;

    public OpeningFileIdentification(Identification id, ModuleSurfaceAreaDocument.ModuleSurfaceArea msa) {
        this.moduleID = id;
        this.xmlMsa = msa;
    }
    
    public OpeningFileIdentification(Identification id, PackageSurfaceAreaDocument.PackageSurfaceArea spd) {
        this.pacakgeID = id;
        this.xmlSpd = spd;
    }
    
    public OpeningFileIdentification(Identification id, FrameworkPlatformDescriptionDocument.FrameworkPlatformDescription fpd) {
        this.moduleID = id;
        this.xmlFpd = fpd;
    }
    
    public Identification getModuleID() {
        return moduleID;
    }

    public void setModuleID(Identification moduleID) {
        this.moduleID = moduleID;
    }

    public Identification getPacakgeID() {
        return pacakgeID;
    }

    public void setPacakgeID(Identification pacakgeID) {
        this.pacakgeID = pacakgeID;
    }

    public Identification getPlatformID() {
        return platformID;
    }

    public void setPlatformID(Identification platformID) {
        this.platformID = platformID;
    }

    public FrameworkPlatformDescriptionDocument.FrameworkPlatformDescription getXmlFpd() {
        return xmlFpd;
    }

    public void setXmlFpd(FrameworkPlatformDescriptionDocument.FrameworkPlatformDescription xmlFpd) {
        this.xmlFpd = xmlFpd;
    }

    public ModuleSurfaceAreaDocument.ModuleSurfaceArea getXmlMsa() {
        return xmlMsa;
    }

    public void setXmlMsa(ModuleSurfaceAreaDocument.ModuleSurfaceArea xmlMsa) {
        this.xmlMsa = xmlMsa;
    }

    public PackageSurfaceAreaDocument.PackageSurfaceArea getXmlSpd() {
        return xmlSpd;
    }

    public void setXmlSpd(PackageSurfaceAreaDocument.PackageSurfaceArea xmlSpd) {
        this.xmlSpd = xmlSpd;
    }
}
