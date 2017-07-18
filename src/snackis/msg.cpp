#include "snackis/ctx.hpp"
#include "snackis/msg.hpp"
#include "snackis/snackis.hpp"
#include "snackis/core/bool_type.hpp"
#include "snackis/core/int64_type.hpp"
#include "snackis/core/uid_type.hpp"
#include "snackis/crypt/pub_key_type.hpp"

namespace snackis {
  const str
  Msg::INVITE("invite"), Msg::ACCEPT("accept"), Msg::REJECT("reject"),
    Msg::POST("post"), Msg::TASK("task");

  Msg::Msg(Ctx &ctx, const str &type): IdRec(ctx), type(type) {
    Peer &me(whoami(ctx));
    from = me.email;
    
    if (type != Msg::REJECT) {
      from_id = me.id;
      peer_name = me.name;
      crypt_key = me.crypt_key;
    }
  }

  Msg::Msg(Ctx &ctx, const db::Rec<Msg> &src): IdRec(ctx, null_uid) {
    db::copy(*this, src);
  }

  str encode(Msg &msg) {
    TRACE("Encoding message");
    Ctx &ctx(msg.ctx);
    const bool encrypt(msg.type != Msg::INVITE &&
		       msg.type != Msg::REJECT);
    
    Stream buf;
    db::Rec<Msg> rec;
    copy(ctx.db.inbox, rec, msg);
    write(ctx.db.inbox, rec, buf, nullopt);
    str data(buf.str());
    
    if (encrypt) {
      Peer pr(get_peer_id(ctx, msg.to_id));
      Data out(crypt::encrypt(*get_val(ctx.settings.crypt_key), pr.crypt_key,
			      reinterpret_cast<const unsigned char *>(data.c_str()),
			      data.size()));
      data.assign(out.begin(), out.end());
    }
    
    buf.str("");
    int64_type.write(PROTO_REV, buf);
    str_type.write(msg.type, buf);

    if (msg.type == Msg::ACCEPT) {
      crypt::pub_key_type.write(msg.crypt_key, buf);
    } else if (encrypt) {
      uid_type.write(msg.from_id, buf);
    }
   
    buf << data;
    data = buf.str();
    
    return bin_hex(reinterpret_cast<const unsigned char *>(data.c_str()),
		   data.size());
  }

  bool decode(Msg &msg, const str &in) {
    TRACE("Decoding message");
    Ctx &ctx(msg.ctx);
    Data data(hex_bin(in));
    InStream in_buf(str(data.begin(), data.end()));

    const int64_t proto_rev(int64_type.read(in_buf));

    if (proto_rev != PROTO_REV) {
      log(msg.ctx, "Protocol revision mismatch");
      return false;
    }

    msg.type = str_type.read(in_buf);
    const bool decrypt(msg.type != Msg::INVITE &&
		       msg.type != Msg::REJECT);
    
    if (decrypt) {
      if (msg.type == Msg::ACCEPT) {
	msg.crypt_key = crypt::pub_key_type.read(in_buf);
      } else {
	msg.from_id = uid_type.read(in_buf);
	auto pr(find_peer_id(ctx, msg.from_id));
	if (!pr) { return false; }
	msg.crypt_key = pr->crypt_key;
      }
      
      data.erase(data.begin(), data.begin()+in_buf.tellg());
      data = crypt::decrypt(*get_val(ctx.settings.crypt_key), msg.crypt_key,
			    &data[0],
			    data.size());
      in_buf.str(str(data.begin(), data.end()));
    }

    db::Rec<Msg> rec;
    db::read(ctx.db.inbox, in_buf, rec, nullopt);
    db::copy(ctx.db.inbox, msg, rec);
    return true;
  }

  void receive(Msg &msg) {
    Ctx &ctx(msg.ctx);

    if (msg.type == Msg::ACCEPT) {
      if (!invited(msg)) { return; }
    } else if (msg.type == Msg::REJECT) {
      if (!invited(msg)) { return; }
    } else if (msg.type == Msg::POST) {
      Feed fd(ctx, msg.feed);
      auto fd_fnd(find_feed_id(ctx, fd.id));
      
      if (fd_fnd) {
	Post ps(ctx, msg.post);
	auto ps_fnd(find_post_id(ctx, ps.id));

	if (ps_fnd) {
	  if (ps_fnd->owner_id != msg.from_id) {
	    log(ctx, "Skipping unauthorized update");
	    return;
	  }
	}
      }
    } else if (msg.type == Msg::TASK) {
      Project prj(ctx, msg.project);
      auto prj_fnd(find_project_id(ctx, prj.id));
      
      if (prj_fnd) {
	Task tsk(ctx, msg.task);
	auto tsk_fnd(find_task_id(ctx, tsk.id));

	if (tsk_fnd) {
	  if (tsk_fnd->owner_id != msg.from_id) {
	    log(ctx, "Skipping unauthorized update");
	    return;
	  }
	}
      }
    }
    
    db::insert(ctx.db.inbox, msg);
  }
}
