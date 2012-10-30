/*
 *  transfer.cpp
 *  some transfer-scope code
 *
 *  Created by Victor Grishchenko on 10/6/09.
 *  Copyright 2009 Delft University of Technology. All rights reserved.
 *
 */
#include <errno.h>
#include <string>
#include <sstream>
#include "swift.h"

#include "ext/seq_picker.cpp" // FIXME FIXME FIXME FIXME 
#include "ext/selector.h"
using namespace swift;

std::vector<FileTransfer*> FileTransfer::files(20);

#define BINHASHSIZE (sizeof(bin64_t)+sizeof(Sha1Hash))

// FIXME: separate Bootstrap() and Download(), then Size(), Progress(), SeqProgress()

FileTransfer::FileTransfer(const char* filename, const Sha1Hash& _root_hash) :
	file_(filename, _root_hash), hs_in_offset_(0), cb_installed(0) {
	if (files.size() < fd() + 1)
		files.resize(fd() + 1);
	files[fd()] = this;
	picker_ = new SeqPiecePicker(this);
	peerSelector = new Selector();
	pFile = fopen("log", "a");
	picker_->Randomize(rand() & 63);
	init_time_ = Datagram::Time();
}

void Channel::CloseTransfer(FileTransfer* trans) {
	for (int i = 0; i < Channel::channels.size(); i++)
		if (Channel::channels[i] && Channel::channels[i]->transfer_ == trans)
			delete Channel::channels[i];
}

void swift::AddProgressCallback(int transfer, ProgressCallback cb, uint8_t agg) {
	FileTransfer* trans = FileTransfer::file(transfer);
	if (!trans)
		return;
	trans->cb_agg[trans->cb_installed] = agg;
	trans->callbacks[trans->cb_installed] = cb;
	trans->cb_installed++;
}

void swift::ExternallyRetrieved(int transfer, bin64_t piece) {
	FileTransfer* trans = FileTransfer::file(transfer);
	if (!trans)
		return;
	trans->ack_out().set(piece); // that easy
}

void swift::RemoveProgressCallback(int transfer, ProgressCallback cb) {
	FileTransfer* trans = FileTransfer::file(transfer);
	if (!trans)
		return;
	for (int i = 0; i < trans->cb_installed; i++)
		if (trans->callbacks[i] == cb)
			trans->callbacks[i] = trans->callbacks[--trans->cb_installed];
}

FileTransfer::~FileTransfer() {
	Channel::CloseTransfer(this);
	fclose(pFile);
	files[fd()] = NULL;
	delete picker_;
}

FileTransfer* FileTransfer::Find(const Sha1Hash& root_hash) {
	for (int i = 0; i < files.size(); i++)
		if (files[i] && files[i]->root_hash() == root_hash)
			return files[i];
	return NULL;
}

int swift::Find(Sha1Hash hash) {
	FileTransfer* t = FileTransfer::Find(hash);
	if (t)
		return t->fd();
	return -1;
}

void FileTransfer::OnPexIn(const Address& addr) {
	for (int i = 0; i < hs_in_.size(); i++) {
		Channel* c = Channel::channel(hs_in_[i]);
		if (c && c->transfer().fd() == this->fd() && c->peer() == addr)
			return; // already connected
	}
	static int tempcounter=0;
	Sha1Hash hash = this->root_hash();
	string test = hash.hex();
	string str(test);
	int port=addr.port();
	if(port!=8000)return;
	if (hs_in_.size() < 20) {
		//modify by futang

		if (maxpeernum == 0) {
			FILE * pFile;
			pFile = fopen("config", "r");
			if (pFile != NULL) {
				char rolec[20] = { 0 };
				char max[20] = { 0 };
				char sortc[20] = { 0 };
				fscanf(pFile, "%s\r\n%s\r\n%s\r\n", rolec, max, sortc);
				string rolestr(rolec);
				string maxstr(max);
				string sortstr(sortc);
				rolestr.erase(0, 5);
				maxstr.erase(0, 7);
				sortstr.erase(0, 5);
				maxpeernum = atoi(maxstr.c_str());
				role = atoi(rolestr.c_str());
				sort = atoi(sortstr.c_str());
				fclose(pFile);
			}
		}
		if (1 == role) {
			peerSelector->AddPeer(addr, hash);
			tempcounter++;
			if(tempcounter<14)return;
			std::vector<Address> peers = peerSelector->GetPeers(sort,
					this->root_hash());
			int counter = peers.size();
			if (counter >= 14) {//five is swarm size
				for (int i = 0; i < maxpeernum; i++) {
					Address addrtemp = (Address) peers[i];
					bool connected = false;
					for (uint j = 0; j < hs_in_.size(); j++) {
						Channel* c = Channel::channel(hs_in_[j]);
						if (c && c->transfer().fd() == this->fd() && c->peer()
								== addrtemp)
							connected = true; // already connected
					}
					if (!connected) {
						new Channel(this, Datagram::default_socket(), addrtemp);
					}
				}
			}

		}

	} else {
		pex_in_.push_back(addr);
		if (pex_in_.size() > 1000)
			pex_in_.pop_front();
	}
}
#if 0
void            FileTransfer::OnPexIn (const Address& addr) {
    for(int i=0; i<hs_in_.size(); i++) {
        Channel* c = Channel::channel(hs_in_[i]);
        if (c && c->transfer().fd()==this->fd() && c->peer()==addr)
            return; // already connected
    }
    if (hs_in_.size()<20) {
        new Channel(this,Datagram::default_socket(),addr);
    } else {
        pex_in_.push_back(addr);
        if (pex_in_.size()>1000)
            pex_in_.pop_front();
    }
}
#endif
int FileTransfer::RevealChannel(int& pex_out_) { // FIXME brainfuck
	pex_out_ -= hs_in_offset_;
	if (pex_out_ < 0)
		pex_out_ = 0;
	while (pex_out_ < hs_in_.size()) {
		Channel* c = Channel::channel(hs_in_[pex_out_]);
		if (c && c->transfer().fd() == this->fd()) {
			if (c->is_established()) {
				pex_out_ += hs_in_offset_ + 1;
				return c->id();
			} else
				pex_out_++;
		} else {
			hs_in_[pex_out_] = hs_in_[0];
			hs_in_.pop_front();
			hs_in_offset_++;
		}
	}
	pex_out_ += hs_in_offset_;
	return -1;
}

