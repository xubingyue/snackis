#include "snackis/ctx.hpp"
#include "snackis/msg.hpp"
#include "snackis/snackis.hpp"

namespace snackis {
  Msg::Msg(Ctx &ctx): Rec(ctx) { }

  Msg::Msg(Ctx &ctx, const str &type, const str &to):
    Rec(ctx), type(type), proto_rev(PROTO_REV), to(to) { }

  Msg::Msg(const db::Table<Msg> &tbl, const db::Rec<Msg> &rec):
    Rec(dynamic_cast<Ctx &>(tbl.ctx)), id(false) {
    copy(tbl, *this, rec);
  }

  str encode(const Msg &msg) {
    Stream buf;
    buf << msg.type << "\r\n";
    db::Rec<Msg> rec;
    copy(msg.ctx.db.outbox, rec, msg);
    write(msg.ctx.db.outbox, rec, buf, nullopt);
    const str data(buf.str());

    if (msg.type == "invite") {
      return
	str("invite\r\n") +
	bin_hex(reinterpret_cast<const unsigned char *>(data.c_str()), data.size());
    }
    
    Data out;
    //TODO: encrypt
    return fmt("%0\r\n%1", msg.type, bin_hex(&out[0], out.size()));
  }
}
