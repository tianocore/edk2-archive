/** @file
 
 The file is used to save basic information of module
 
 Copyright (c) 2006, Intel Corporation
 All rights reserved. This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution.  The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php
 
 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 
 **/

package org.tianocore.frameworkwizard.module;


import org.tianocore.frameworkwizard.common.Identification;
import org.tianocore.frameworkwizard.packaging.PackageIdentification;

public class ModuleIdentification extends Identification {
    
    private PackageIdentification packageId;
    
    public ModuleIdentification(String name, String guid, String version, String path) {
    	super(name, guid, version, path);
    }
    
    public ModuleIdentification(String name, String guid, String version, PackageIdentification packageId){
        super(name, guid, version);
        this.packageId = packageId;
    }
    
    public boolean equals(Object obj) {
        if (obj instanceof Identification) {
            Identification id = (Identification)obj;
            if ( this.getName().equalsIgnoreCase(id.getName())) {
                return true;
            }
            // and package is the same one
            return false;
        }
        else {
            return super.equals(obj);
        }
    }
    
    public String toString(){
        return "Module " + this.getName() + "[" + this.getGuid() + "] in package " + packageId;
    }

    public void setPackageName(PackageIdentification packageId) {
        this.packageId = packageId;
    }
}
