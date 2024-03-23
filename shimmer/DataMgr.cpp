#include "stdafx.h"
#include "DataMgr.h"
#include <uv.h>


void CDataMgr::pull() {

}

void CDataMgr::update() {
	printf("afasfasdfasdafasdf");
}

std::deque<server_list_t> CDataMgr::get_serer_list() {
	return server_list;
}
