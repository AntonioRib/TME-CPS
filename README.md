# Trusted and Privacy Enhanced Cloud Computing Environments (trustedCCEnvironment)

This repository includes all the code for the Trusted Cloud Computing Environment Prototype built for the MSc. Thesis Trusted and Privacy Enhanced Cloud Computing Environments [[1](#references)], whose purpose is to design and implement a system flexible enough to let all the needed management activities be done while at the same time protecting user data privacy conditions, and availability guarantees. 

## Context

Cloud-based PaaS and SaaS solutions are being increasingly used as new extensions of services offered by different cloud-service providers. Such platforms or ready-to-use applications and services provide an easy way for developers to deploy, integrate or run complete outsourced services, at relatively low cost and low effort. These offers include valuable characteristics to address scalability, availability, elasticity, ubiquity and pay-peruse cost models. However, such environments, are managed by administrators, which operations are not controlled or audited by the users. These systems should make sure that management users (for example System Administrators) have no access to the private data being handled in different cloud computing nodes; therefore keeping the data from the end-users safe. 

The current cloud models entail security and privacy risks for its customers. PaaS providers are entrusted with applications, users’ computations, as as well as potentially confidential data processed, which may be leaked or tampered, as a result of ill-configured cloud infrastructure software, as well as, incorrect or not-well verified operations. 

We purpose an architecture for trusted maintenance protocols to mitigate potential malicious or incorrect actions from systems’ administrators of PaaS and SaaS solutions. Our solution provides a secure and trustable environment, where privacy-enhanced applications and services can be supported on top of the current PaaS software-stack architectures. It also combines support for management multi-roles involving system administrators and auditing teams, in possible segregated administrative domains, while also supporting external auditing functions for independent auditors and possible verification’s from end-users. The key insight is that instead of never granting full administrative privileges on the computing nodes, we will allow the administrative personal the necessary privileges according to escalation policies enforced for multiple and segregated roles.

<!-- The designed solution supports cloud system management functions and protocols, running with trustworthy guarantees supported in a trustable execution environment leveraged by Intel SGX enabled processors for trusted execution and software attestation guarantees.

For our objectives we focused on the main contributions aligned with the following
requirements:

* Support for internal multi-role based operational and auditing teams, including
support for operational teams managing nodes hosting customers’ computations,
and auditing team members responsible for auditing and supervising their actions;

* Auditing teams may have members hired by the provider, but also external members
hired by customers, or designated by external regulation entities, whenever
necessary, with the possible support of remote-management auditing protocols executed
in a trustable way;

* Control of Operational States for nodes considered as trusted nodes to run customer’s
computations or workloads;

* Remote attestation protocols running under trustworthy guarantees, certifying the
relevant boot sequence of PaaS or SaaS stacks in systems’ configurations;

* Trustworthy guarantees of state-machine executions in supporting the management
protocols and functions. This includes event-logging of changes in operational
states of nodes, with procedures involved to assure the required liability and attestation
guarantees. -->

## Architecture
The following figure depicts the system architecture of our prototype:

![System Architecture](https://github.com/AntonioRib/trustedCCEnvironment/blob/master/READMEFiles/SystemArchitecture.png)

It is composed by three Clusters which all have different purposes on the infrastructure.
* The Cluster of Nodes called Auditing & Logging Hub takes care of logging the actions of administrators on nodes on the Execution Cluster. This Cluster is the entrance of any System Administrator to work on the System. Also, all the other nodes in the other Clusters have mechanism to responsible for supporting the logging system. 
* The System Management Cluster takes care of the management and orchestration of the Execution Cluster, meaning that it is where nodes for the Execution Cluster are initiated, removed or have their status changed. We refer to this nodes as Monitors
* The Execution Cluster is divided onto two, part of it is dedicated for Cloud Services (such as Databases), and Cloud Applications which are the applications uploaded by Developers for the use of End-users. We refer to the nodes running the Developers applications (therefore the Nodes on the Execution Cluster) as Minions.

In our prototype the software running on these nodes has both implementations with and without Intel SGX. (The Intel SGX implementations have the SGX sufix eg. MonitorSGX). Moreover, all the actors represented have their software which lets them make their actions.


## Pre-Requisites

It is recommended the prototype is tested on Linux environments.

To compile and/or run the prototypes its needed,
* C++11
* GCC 7.3.0 - [Here](https://gcc.gnu.org/)
* OpenSSH 7.6 - [Here](https://www.openssh.com/)
* Docker 18.06.1-ce - [Here](https://www.docker.com/)
* TPM2 Software Stack (TSS2) and it's dependencies - [Here](https://github.com/tpm2-software/tpm2-tss)
* TPM2 Access Broker & Resource Management Daemon and it's dependencies - [Here](https://github.com/tpm2-software/tpm2-abrmd)
* _(If you dont have an Hardware with TPM2)_ TPM2 Simulator and it's dependencies - [Here](https://sourceforge.net/projects/ibmswtpm2/files/ibmtpm974.tar.gz/download?use_mirror=datapacket)
* spdlog a Fast C++ logging library - [Here](https://github.com/gabime/spdlog)
* sTunnel configured with the configuration files from [here](https://github.com/AntonioRib/trustedCCEnvironment/tree/master/sTunnel)

To compile and run the software compatible with [Intel SGX](https://01.org/intel-softwareguard-extensions) additionally you need,
* Intel(R) SGX Driver for Linux and it's dependencies - [Here](https://github.com/intel/linux-sgx)
* Intel(R) SGX Platform Software (PSW) for Linux and it's dependencies - [Here](https://github.com/intel/linux-sgx)

## Instructions

The prototype's software can be found in the src folder.

### Compiling the code

After installing all the pre-requisites. All the software can be compiled at once through the use of ``` make ``` on the root folder. Each software can also be compiled separately on it's specific folder with the use of ``` make ```

To compile the prototype using Intel SGX in Simulation mode it's only needed to run 

    make

To compile the prototype using Intel SGX in Harware mode it's needed to be compiled with the flag

    make SGX_MODE=HW

After this, the enclave file should be signed with the enclave signer of Intel SGX SDK. In case you want to skip the signing you can compile the software with the flags

    make SGX_MODE=HW SGX_PRERELEASE=1

This will automatically sign the enclaves with the certificates on this repository. __CAUTION:__ Even thought Pre-Release mode is the same as Releas Mode with regard to optimization and debug symbol support, the enclave will be launched in enclave-debug mode, which is suitable for performance testings but not for production builds or final product releases [[2](#references)]. 

### Running the software

Before running the software, the file ``` createSystem.sh ``` should be run. This file will prepare the file system how the software expects it.

After compiling the Client softwares will be built onto a the folder ``` \bin ```. The Servers softwares will be ready to use on each server folder ``` \src\Servers\<softwareFolder> ``` eg. ``` \src\Servers\Monitor ```.

All the software has a help menu where they show the available commands. The next sections also show how to start each software with a bit more explanations.

#### Servers
##### Monitor

    Usage: Monitor -u username -j sshKey -h host OR MonitorSGX -u username -j sshKey -h host

This will start the Monitor software. The -j parameter with the sshKey is deprecated and it's not used.

##### Auditing Hub

    Usage: AudtingHub -u userName -k key -m monitor -h hostname OR AudtingHubSGX -u userName -k key -m monitor -h hostname

This will start the Auditing Hub software. The -k parameter with the key is deprecated and it's not used. The -u with the username parameter is used to execute the SSH commands (therefore needs a user with enough permissions).

##### Minion

    Usage: Minion -m monitorHostName -h hostname -i ipAddress OR MinionSGX -m monitorHostName -h hostname -i ipAddress
    
This will start the Minion software.

#### Clients
##### Auditor & AuditorMinion 

These don't need any specific parameters.

##### Auditor Interface 

    Usage: AuditorInterface -m monitor -h hostName

This will start the Auditor Interface software for the Auditing Hub where we are able to list, read and commit the logs of the administration sessions.

##### Developer

    Usage to Deploy App: Developer -deploy -m monitorHost -u username -k key -a appDir -i #instances
    Usage to Delete App: Developer -delete -m monitorHost -u username -k key -a appDir

The -k parameter with the key is deprecated and it's not used. The -u with the username parameter is used to execute the SCP commands (therefore needs a user with enough permissions). The -a parameter should point to a Dockerfile, which is what will be uploaded to the servers.

##### SysAdmin

    Usage for Normal Session: SysAdmin -a hub -h host -u username -k key
    Usage for Urgent Session: SysAdmin -a hub -h host -u username -k key -force

The -k parameter with the key is deprecated and it's not used. The -u with the username parameter is used to execute the SSH commands (therefore needs a user with enough permissions). This software will start a proxy-SSH session on the specified Minion (with the flag ``` -h host ```), and to stop it the user needs to write ``` QUIT ```

## Open Issues

* Some software parameters are left from previous implementations and are therefore useless.
* Besides deploying the prototype the accesses and users of the OS where it runs needs to be correctly configured for the experience to be complete.
* For the prototype to be complete as on the source it needs sTunnel running with [Graphene-SGX](https://github.com/oscarlab/graphene/wiki/Introduction-to-Intel-SGX-Support)

## References

1. A. Ribeiro. Management of Trusted and Privacy Enhanced Cloud Computing Environments, MSc Thesis. Universidade Nova de Lisboa, 2019.
2. Intel® SGX: Debug, Production, Pre-release --What's the Difference?: https://software.intel.com/en-us/blogs/2016/01/07/intel-sgx-debug-production-prelease-whats-the-difference


## Contact

For any questions or suggestions you may have, please get in touch with:
* António Ribeiro, [am.ribeiro@campus.fct.unl.pt](mailto:am.ribeiro@campus.fct.unl.pt)
* Henrique Domingos, [hj@fct.unl.pt](mailto:hj@fct.unl.pt)