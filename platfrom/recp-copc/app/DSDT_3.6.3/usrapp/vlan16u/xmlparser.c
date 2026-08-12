/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @Date: 2019-10-22 14:16:42
 * @LastEditors: smith.zj
 * @LastEditTime: 2020-04-07 13:56:07
 */
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/xmlmemory.h>
#include <libxml/xpointer.h>
#include "common_log.h"
#include <string.h>
#include "xmlparser.h"
#include "netcfg.h"
#include <errno.h>

RET_STATUS parse_port_cfg_node(xmlNodePtr node,net_port_cfg_s *port_cfg);
RET_STATUS parse_port_cfg_node(xmlNodePtr node,net_port_cfg_s *port_cfg)
{
	xmlNodePtr childcur = node;
	xmlChar* szAttr = NULL;
	char *stop = NULL;	
	char *s;
	char *token;
	int i;

	PRINT( "portcfg node:\n");
	while(childcur != NULL) {
		szAttr = xmlNodeGetContent(childcur);
		PRINT( "\tname %s contnet %s\n",childcur->name,szAttr);
		if(!xmlStrcmp(childcur->name, BAD_CAST("qmode"))) {
			port_cfg->qmode = strtoul((char *)szAttr, &stop, 10);
			if (*stop != '\0') {
				PRINT( "no qmode set,used default config\n");
				port_cfg->qmode = NET_8021Q_NONE;
			}
		} else if(!xmlStrcmp(childcur->name, BAD_CAST("pvid"))) {
			port_cfg->pvid = strtoul((char *)szAttr, &stop, 10);
			if (*stop != '\0') {
				PRINT( "no pvid set,used default config\n");
				port_cfg->pvid = NET_DEFAULT_PVID;
			}
		} else if(!xmlStrcmp(childcur->name, BAD_CAST("portbasevlan"))) {
			s = strdup((char *)szAttr);
			i = 0;
			for(token = strsep(&s, ","); token != NULL; token = strsep(&s, " ,")) {
				/*port_cfg->portVectormap[i++] = atoi(token);*/
				port_cfg->portVectormap[i] = (uint32_t)strtoul(token, &stop, 10);
				if (*stop != '\0') {
					PRINT( "get port failed(%s)\n",strerror(errno));
				}
				i++;
			}
			port_cfg->port_map_num = i;
		}
		xmlFree(szAttr);
		childcur=childcur->next;
	}

	return NET_RET_OK;
}

RET_STATUS parse_port_cfg(xmlNodeSetPtr nodeset, net_port_cfg_s **port_cfg,uint16_t *port_cfg_len)
{
	int i = 0;
	char *stop = NULL;	
	xmlNodePtr cur;
	xmlChar* xml_portid = NULL;
	net_port_cfg_s * tmp_port_cfg;

	RET_STATUS status = NET_RET_OK;

	PRINT("nodeset->nodeNr %d\n",nodeset->nodeNr);
	*port_cfg_len = nodeset->nodeNr;
	tmp_port_cfg = (net_port_cfg_s *)calloc( *port_cfg_len,sizeof(net_port_cfg_s));
	if(tmp_port_cfg == NULL) {
		DB_PRINT(DB_ERR,"calloc mem for port_cfg failed(size %d)\n",sizeof(net_port_cfg_s)*(*port_cfg_len));
		return NET_RET_FAIL;
	}
	*port_cfg = tmp_port_cfg;

    for (i = 0; i < nodeset->nodeNr; i++) { 
        cur = nodeset->nodeTab[i];
        /*解析*/
		xml_portid = xmlGetProp(cur,BAD_CAST "id");
		PRINT("portid is %s\n",xml_portid);
		tmp_port_cfg[i].port = strtoul((char *)xml_portid, &stop, 10);
		if (*stop != '\0') {
			DB_PRINT(DB_ERR,"port id invalid(%s)\n",xml_portid);
			status = NET_RET_FAIL;
			goto PARSE_PORTCFG_FAILED;
		}
		status = parse_port_cfg_node(cur->children,&tmp_port_cfg[i]);
		if(status != NET_RET_OK) {
			DB_PRINT(DB_ERR,"parse_port_cfg_node failed\n");
			status = NET_RET_FAIL;
			goto PARSE_PORTCFG_FAILED;
		}
    }
	return status;

PARSE_PORTCFG_FAILED:
	free(tmp_port_cfg);
	return status;
}


RET_STATUS parse_vlan_entry_node(xmlNodePtr node,net_service_vlan_cfg_s *vlan_entry_cfg);
RET_STATUS parse_vlan_entry_node(xmlNodePtr node,net_service_vlan_cfg_s *vlan_entry_cfg)
{
	xmlNodePtr childcur = node;
	xmlChar* szAttr = NULL;
	char *stop = NULL;	
	char *s;
	char *token;
	int i;

	PRINT( "vlanentry node:\n");
	while(childcur != NULL) {
		szAttr = xmlNodeGetContent(childcur);
		PRINT( "\tname %s contnet %s\n",childcur->name,szAttr);
		if(!xmlStrcmp(childcur->name, BAD_CAST("vid"))) {
			vlan_entry_cfg->vlan_entry.vid = strtoul((char *)szAttr, &stop, 10);
			if (*stop != '\0') {
				PRINT( "no vid set,may be need calc func\n");
			}
		} else if(!xmlStrcmp(childcur->name, BAD_CAST("sid"))) {
			vlan_entry_cfg->vlan_entry.sid = strtoul((char *)szAttr, &stop, 10);
			if (*stop != '\0') {
				PRINT( "no sid set\n");
			}
		} else if(!xmlStrcmp(childcur->name, BAD_CAST("fid"))) {
			vlan_entry_cfg->vlan_entry.fid = strtoul((char *)szAttr, &stop, 10);
			if (*stop != '\0') {
				PRINT( "no fid set\n");
			}
		}  else if(!xmlStrcmp(childcur->name, BAD_CAST("vidPolicy"))) {
			vlan_entry_cfg->vlan_entry.vidPolicy = strtoul((char *)szAttr, &stop, 10);
			if (*stop != '\0') {
				PRINT( "no vidPolicy set\n");
			}
		}  else if(!xmlStrcmp(childcur->name, BAD_CAST("calcfunc"))) {
			vlan_entry_cfg->calc_vlan_funcid = strtoul((char *)szAttr, &stop, 10);
			PRINT("vlan_entry_cfg %d\n",vlan_entry_cfg->calc_vlan_funcid);
			if (*stop != '\0') {
				PRINT( "no calcfunc_id set\n");
			} else {
				vlan_entry_cfg->slot_relative = true;
			}
		} else if (!xmlStrcmp(childcur->name, BAD_CAST("memberport"))) {
			s = strdup((char *)szAttr);
			i = 0;
			for(token = strsep(&s, ","); token != NULL; token = strsep(&s, " ,")) {
				vlan_entry_cfg->vlan_entry.memberTagP[i++] = (uint32_t)strtoul(token, &stop, 10);
				if (*stop != '\0') {
					PRINT( "get memberTagP failed(%s)\n",strerror(errno));
				}
			}
		}

		xmlFree(szAttr); 
		childcur=childcur->next;
	}
	if(vlan_entry_cfg->vlan_entry.vid == 0 && vlan_entry_cfg->calc_vlan_funcid == 0) {
		DB_PRINT(DB_ERR,"FATAL:you must specific a vid or give calc_vlan_funcid");
		return NET_RET_FAIL;
	}
	if(vlan_entry_cfg->calc_vlan_funcid >= MAX_CALC_VLAN_FUNC_NUM) {
		DB_PRINT(DB_ERR,"FATAL:calc_vlan_funcid over range(1~%d)",MAX_CALC_VLAN_FUNC_NUM);
	}

	return NET_RET_OK;
}


RET_STATUS parse_vlan_entry_cfg(xmlNodeSetPtr nodeset, net_service_vlan_cfg_s **vlan_cfg,uint16_t *vlan_cfg_len);
RET_STATUS parse_vlan_entry_cfg(xmlNodeSetPtr nodeset, net_service_vlan_cfg_s **vlan_cfg,uint16_t *vlan_cfg_len)
{
	int i = 0;
	xmlNodePtr cur;
	net_service_vlan_cfg_s * tmp_vlan_cfg;
	RET_STATUS status = NET_RET_OK;

	PRINT("vlan node %d\n",nodeset->nodeNr);
	*vlan_cfg_len = nodeset->nodeNr;
	tmp_vlan_cfg = (net_service_vlan_cfg_s *)calloc( *vlan_cfg_len, sizeof(net_service_vlan_cfg_s));
	if(tmp_vlan_cfg == NULL) {
		DB_PRINT(DB_ERR,"calloc mem for port_cfg failed(size %d)\n",sizeof(net_service_vlan_cfg_s)*(*vlan_cfg_len));
		return NET_RET_FAIL;
	}
	*vlan_cfg = tmp_vlan_cfg;

    for (i = 0; i < nodeset->nodeNr; i++) { 
        cur = nodeset->nodeTab[i];
        /*解析*/
		status = parse_vlan_entry_node(cur->children,&tmp_vlan_cfg[i]);
		if(status != NET_RET_OK) {
			DB_PRINT(DB_ERR,"parse_port_cfg_node failed\n");
			status = NET_RET_FAIL;
			goto PARSE_VLANENTRY_FAILED;
		}
    }
	return status;

PARSE_VLANENTRY_FAILED:
	free(tmp_vlan_cfg);
	return status;	
}

RET_STATUS load_netdescriptor(const char *board_desc, uint16_t *smiaddr,net_port_cfg_s **port_cfg, uint16_t *port_cfg_len,\
            net_service_vlan_cfg_s **vlan_cfg,uint16_t *vlan_cfg_len);
RET_STATUS load_netdescriptor(const char *board_desc, uint16_t *smiaddr,net_port_cfg_s **port_cfg, uint16_t *port_cfg_len,\
            net_service_vlan_cfg_s **vlan_cfg,uint16_t *vlan_cfg_len)
{
	xmlDocPtr pdoc = NULL;
	xmlNodePtr proot = NULL;
	xmlXPathContextPtr context=NULL;
	xmlXPathObjectPtr result=NULL; 

	char xpath_buf[100]= {0};
	xmlChar* szAttr = NULL;
	xmlNodeSetPtr nodeset;
	char *stop;
	*port_cfg_len = 0;
	*vlan_cfg_len = 0;
	/*****************打开xml文档********************/
	xmlKeepBlanksDefault(0);/*必须加上，防止程序把元素前后的空白文本符号当作一个node*/
	pdoc = xmlReadFile (XML_DESCRIPTOR_FILE, "GB2312", XML_PARSE_RECOVER);/*libxml只能解析UTF-8格式数据*/
	if (pdoc == NULL) {
		DB_PRINT(DB_ERR,"ERROR:can't open file!");
		return NET_RET_BAD_PARAM;
	}
	 
	/*****************获取xml文档对象的根节对象********************/
	proot = xmlDocGetRootElement (pdoc);	
	if (proot == NULL) {
		DB_PRINT(DB_ERR,"ERROR: file is empty!\n");
		return NET_RET_FAIL;
	}
    context = xmlXPathNewContext(pdoc);
	if (context == NULL) {
			DB_PRINT(DB_ERR,"ERROR:context is NULL\n");
			return NET_RET_FAIL;
	}

	/*****************查询smi 地址********************/
	sprintf(xpath_buf,NET_CFG_SMIADDR_XPATH,board_desc);
	result = xmlXPathEvalExpression(BAD_CAST(xpath_buf), context);
	if (result->nodesetval == NULL) {
		DB_PRINT(DB_ERR,"ERROR:xmlXPathEvalExpression return NULL(xpath:%s)\n",xpath_buf);
	} else {
        xmlNodeSetPtr nodeset = result->nodesetval; /*获取查询到的节点指针集合*/
        szAttr = xmlNodeGetContent(nodeset->nodeTab[0]);
		*smiaddr = strtoul((char *)szAttr, &stop, 16);
		if (*stop != '\0') {
			PRINT( "no smiaddr set\n");
		}
        PRINT( "SMIADDR %s\n",szAttr);
        xmlFree(szAttr); 
    }
    /*****************获取端口配置********************/
    sprintf(xpath_buf,NET_CFG_PORTCFG_XPATH,board_desc); 
	result = xmlXPathEvalExpression(BAD_CAST(xpath_buf), context);
	if (result->nodesetval == NULL) {
		DB_PRINT(DB_ERR,"ERROR:xmlXPathEvalExpression return NULL(xpath:%s)\n",xpath_buf);
	} else {
		nodeset = result->nodesetval; 
		if(parse_port_cfg(nodeset,port_cfg,port_cfg_len) != NET_RET_OK) {
			DB_PRINT(DB_ERR,"parse_port_cfg failed\n");
			return NET_RET_FAIL;
		}
	}



	/*****************获取vlan entry********************/
    sprintf(xpath_buf,NET_CFG_VLANENTRY_XPATH,board_desc); 
	result = xmlXPathEvalExpression(BAD_CAST(xpath_buf), context);
	if (result->nodesetval == NULL) {
		DB_PRINT(DB_ERR,"ERROR:xmlXPathEvalExpression return NULL(xpath:%s)\n",xpath_buf);
	} else {
		nodeset = result->nodesetval; 
		if(parse_vlan_entry_cfg(nodeset,vlan_cfg,vlan_cfg_len) != NET_RET_OK) {
			DB_PRINT(DB_ERR,"parse_port_cfg failed\n");
			return NET_RET_FAIL;
		}
	}


	/*****************释放资源********************/
	xmlXPathFreeContext(context); /*释放上下文指针*/
	xmlFreeDoc (pdoc);
	xmlCleanupParser ();
	xmlMemoryDump ();
	
	return NET_RET_OK;
}