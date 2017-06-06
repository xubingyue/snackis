#include "snackis/ctx.hpp"
#include "snackis/msg.hpp"
#include "snackis/snackis.hpp"

namespace snackis {
  const str Msg::INVITE("invite"), Msg::ACCEPT("accept"), Msg::REJECT("reject");

  Msg::Msg(Ctx &ctx): Rec(ctx) { }

  Msg::Msg(Ctx &ctx, const str &type, const str &to):
    Rec(ctx), type(type), to(to) { }

  Msg::Msg(const db::Table<Msg> &tbl, const db::Rec<Msg> &rec):
    Rec(dynamic_cast<Ctx &>(tbl.ctx)), id(false) {
    copy(tbl, *this, rec);
  }

  str encode(const Msg &msg) {
    Ctx &ctx(msg.ctx);
    
    Stream buf;
    db::Rec<Msg> rec;
    copy(ctx.db.inbox, rec, msg);
    write(ctx.db.inbox, rec, buf, nullopt);
    const str data(buf.str());

    if (msg.type == Msg::INVITE) {
      return
	fmt("%0\r\n%1\r\n%2", PROTO_REV, Msg::INVITE,
	    bin_hex(reinterpret_cast<const unsigned char *>(data.c_str()),
		    data.size()));
    }

    Peer peer(get_email_peer(ctx, msg.to));
    Data out(crypt::encrypt(*get_val(ctx.settings.crypt_key), peer.crypt_key,
			    reinterpret_cast<const unsigned char *>(data.c_str()),
			    data.size()));

    return fmt("%0\r\n%1\r\n%2\r\n%3",
	       PROTO_REV, msg.type, msg.from, bin_hex(&out[0], out.size()));
  }

  bool decode(Msg &msg, const str &in) {
    auto i(in.find("\r\n"));
    if (i == str::npos) { return false; }
    int64_t proto_rev = to_int64(in.substr(0, i));
    
    if (proto_rev != PROTO_REV) {
      log(msg.ctx, "Failed decoding message due to protocol revision mismatch");
      return false;
    }
    
    i += 2;

    auto j(in.find("\r\n", i));
    if (j == str::npos) { return false; }
    msg.type = in.substr(i, j-i);
    i = j+2;

    if (msg.type != Msg::INVITE) {
      j = in.find("\r\n", i);
      if (j == str::npos) { return false; }
      msg.from = in.substr(i, j-i);
      i = j+2;
    }

    Ctx &ctx(msg.ctx);
    Data dat(hex_bin(in.substr(i)));

    if (msg.type != Msg::INVITE) {
      const Peer peer(get_email_peer(ctx, msg.from));
      dat = crypt::decrypt(*get_val(ctx.settings.crypt_key), peer.crypt_key,
			   &dat[0],
			   dat.size());
    }

    Stream buf(str(dat.begin(), dat.end()));
    db::Rec<Msg> rec;
    read(ctx.db.inbox, buf, rec, nullopt);
    copy(ctx.db.inbox, msg, rec);
    return true;
  }
}
