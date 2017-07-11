#include "snackis/ctx.hpp"
#include "snackis/msg.hpp"
#include "snackis/snackis.hpp"
#include "snackis/core/bool_type.hpp"
#include "snackis/core/int64_type.hpp"
#include "snackis/core/uid_type.hpp"

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

  str encode(const Msg &msg) {
    TRACE("Encoding message");
    Ctx &ctx(msg.ctx);
    const bool encrypt(msg.type != Msg::INVITE &&
		       msg.type != Msg::ACCEPT &&
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
    bool_type.write(encrypt, buf);
    if (encrypt) { uid_type.write(msg.from_id, buf); }
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
    
    const bool decrypt(bool_type.read(in_buf));
    
    if (decrypt) {
      msg.from_id = uid_type.read(in_buf);
      auto pr(find_peer_id(ctx, msg.from_id));
      if (!pr) { return false; }
      
      data.erase(data.begin(), data.begin()+in_buf.tellg());
      data = crypt::decrypt(*get_val(ctx.settings.crypt_key), pr->crypt_key,
			    &data[0],
			    data.size());
      in_buf.str(str(data.begin(), data.end()));
    }

    db::Rec<Msg> rec;
    db::read(ctx.db.inbox, in_buf, rec, nullopt);
    db::copy(ctx.db.inbox, msg, rec);
    return true;
  }

  bool receive(Msg &msg) {
    Ctx &ctx(msg.ctx);

    if (msg.type == Msg::POST) {
      Feed fd(ctx, msg.feed);
      auto fd_fnd(find_feed_id(ctx, fd.id));
      
      if (fd_fnd) {
	Post ps(ctx, msg.post);
	auto ps_fnd(find_post_id(ctx, ps.id));

	if (ps_fnd) {
	  Post prev(*ps_fnd);
	  copy(*ps_fnd, msg);
	  
	  if (db::compare(ctx.db.posts, prev, *ps_fnd) == 0) {
	    log(ctx, "Skipped duplicate post update");
	    return false;
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
	  Task tsk_prev(*tsk_fnd);
	  copy(*tsk_fnd, msg);
	  
	  if (db::compare(ctx.db.tasks, tsk_prev, *tsk_fnd) == 0) {
	    log(ctx, "Skipped duplicate task update");
	    return false;
	  }
	}
      }
    }
    
    db::insert(ctx.db.inbox, msg);
    return true;
  }
}
